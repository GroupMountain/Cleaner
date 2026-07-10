#include "Cleaner.h"
#include "Global.h"
#include "Mod.h"

#include "ll/api/base/Containers.h"
#include "ll/api/service/Bedrock.h"
#include "mc/common/Globals.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/storage/DBStorage.h"
#include <mc/world/actor/provider/SynchedActorDataAccess.h>

#include <gmlib/mc/world/actor/UnloadedActor.h>
#include <ll/api/form/CustomForm.h>
#include <ll/api/form/ModalForm.h>
#include <ll/api/form/SimpleForm.h>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

using namespace std::ranges;

extern struct OfflineEntityData {
    std::chrono::system_clock::time_point lastUpdate{};
    std::atomic_bool                      isUpdating{false};
    std::unordered_map<std::string, std::vector<gmlib::UnloadedActor>> offlineEntities{};
    ll::SmallDenseSet<ActorUniqueID>      waitedPlayers{};
    std::atomic_bool                      requireUpdate{false};
    std::mutex                            offlineEntitiesMutex;
} OfflineEntity;

bool shouldClean(gmlib::GMActor* actor,
    bool checkTamed, bool checkTrusting, bool checkCustomName,
    bool checkTag, bool checkItemAge, bool checkItemType
) {
    auto& cfg = Cleaner::Entry::getInstance().getConfig();

    if (checkTamed && actor->isTame()) return false;
    if (checkTrusting && SynchedActorDataAccess::getActorFlag(actor->getEntityContext(), ActorFlags::Trusting)) return false;
    if (checkCustomName && !actor->getNameTag().empty()) return false;
    if (checkTag) {
        if (actor->hasTag("cleaner:ignore") || any_of(cfg.IgnoreTags, [&](auto& t) { return actor->hasTag(t); }))
            return false;
    }

    if (actor->hasCategory(::ActorCategory::Item)) {
        auto itac = (ItemActor*)actor;
        if (checkItemAge && itac->age() <= cfg.CleanItem.ExistTicks) {
            return false;
        }
        auto itemType = itac->item().getTypeName();
        auto whitelist = cfg.CleanItem.Whitelist;
        if (checkItemType) {
            for (auto& key : whitelist) {
                if (Cleaner::isMatch(itemType, key)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool shouldCleanUnload(const gmlib::UnloadedActor& actor,
    bool checkTamed, bool checkTrusting, bool checkCustomName,
    bool checkTag, bool checkItemAge, bool checkItemType
) {
    auto nbt = actor.getNbt();
    if (!nbt) return false;

    auto& cfg = Cleaner::Entry::getInstance().getConfig();

    auto getByte = [&](std::string_view key) -> char {
        return nbt->contains(key) ? nbt->at(key).get<ByteTag>().data : 0;
    };
    if (checkTamed && getByte("IsTamed")) return false;
    if (checkTrusting && getByte("IsTrusting")) return false;
    if (checkCustomName && getByte("CustomNameVisible")) return false;

    if (checkTag && nbt->contains("Tags")) {
        auto& tags = nbt->at("Tags").get<ListTag>();
        for (auto& t : tags | views::filter([](auto& v) { return v->as<StringTag>().data() != nullptr; })) {
            auto tag = t->as<StringTag>().data();
            if (tag == "cleaner:ignore" || any_of(cfg.IgnoreTags, [&](auto& kt) { return tag == kt; }))
                return false;
        }
    }

    if (actor.getTypeName() == "minecraft:item") {
        if (checkItemAge && nbt->contains("Age") && nbt->at("Age").get<ShortTag>().data <= cfg.CleanItem.ExistTicks)
            return false;
        if (checkItemType && nbt->contains("Item")) {
            auto& itemNbt = nbt->at("Item").get<CompoundTag>();
            if (itemNbt.contains("Name")) {
                std::string itemType = itemNbt.at("Name").get<StringTag>().data();
                auto whitelist = cfg.CleanItem.Whitelist;
                for (auto& key : whitelist) {
                    if (Cleaner::isMatch(itemType, key)) {
                        return false;
                    }
                }
            }
        }

    }
    return true;
}

bool notCooldown() {
    return OfflineEntity.lastUpdate + std::chrono::minutes(10) >= std::chrono::system_clock::now();
}

auto typeNameToLocString(std::string const& typeName) {
    return EntityTypeToLocString(
        EntityTypeFromString(typeName),
        ActorTypeNamespaceRules::ReturnWithoutNamespace
    );
}

auto getAllEntityTypes() {
    return to<ll::SmallDenseSet<std::string>>(
        ll::service::getLevel()->getRuntimeActorList()
        | views::transform(&Actor::getTypeName)
    );
}

auto getEntityCountByType(std::string const& typeName) {
    return count_if(ll::service::getLevel()->getRuntimeActorList(),
        [&](const Actor* a) { return a && a->getTypeName() == typeName; });
}

void ConfirmForm(Player& player, bool isOnline, const std::string& TypeName,
    bool checkTamed, bool checkTrusting, bool checkCustomName,
    bool checkTag, bool checkItemAge, bool checkItemType
) {
    auto fm = ll::form::ModalForm(
        tr("cleaner.form.confirm.title"),
        tr(isOnline ? "cleaner.form.confirm.important" : "cleaner.form.confirm.warning", {
            tr(typeNameToLocString(TypeName)), TypeName,
            std::to_string(isOnline ? getEntityCountByType(TypeName) : OfflineEntity.offlineEntities[TypeName].size())
        }),
        tr("cleaner.form.confirm.confirm"), tr("cleaner.form.confirm.cancel")
    );

    fm.sendTo(player, [=](const Player& p, ll::form::ModalFormResult rst, ll::form::FormCancelReason res) {
        if (res || !rst || rst.value() != ll::form::ModalFormSelectedButton::Upper) return;

        if (isOnline) {
            for (auto* en : ll::service::getLevel()->getRuntimeActorList()) {
                if (en->getTypeName() == TypeName && shouldClean((gmlib::GMActor*)en,
                    checkTamed, checkTrusting, checkCustomName, checkTag, checkItemAge, checkItemType)) {
                    en->despawn();
                }
            }
        } else{
            std::thread([=]() {
                {
                    std::lock_guard<std::mutex> lock(
                        OfflineEntity.offlineEntitiesMutex
                    );
                    std::vector<gmlib::UnloadedActor> unloadActors;
                    auto it = OfflineEntity.offlineEntities.find(TypeName);
                    if (it != OfflineEntity.offlineEntities.end()) {
                        for (auto& en : it->second) {
                            if (en.isValid()&&(
                                shouldCleanUnload(en,checkTamed,checkTrusting,checkCustomName,checkTag,checkItemAge,checkItemType)))
                                en.remove();
                            else unloadActors.emplace_back(std::move(en));
                        }
                        OfflineEntity.offlineEntities[it->first].swap(unloadActors);
                    }
                }

            }).detach();
        }
    });
}

void SettingCleanForm(Player& player, bool isOnline, const std::string& TypeName) {
    auto fm = ll::form::CustomForm(tr("cleaner.form.setting.title"));
    fm.appendLabel(tr("cleaner.form.setting.content", {
        tr(typeNameToLocString(TypeName)), TypeName,
        std::to_string(isOnline ? getEntityCountByType(TypeName) : OfflineEntity.offlineEntities[TypeName].size())
    }));

    const auto toggles = {
        std::pair{"cleanTamed", tr("cleaner.form.setting.cleanTamed")},
        {"cleanTrusting", tr("cleaner.form.setting.cleanTrusting")},
        {"cleanCustomName", tr("cleaner.form.setting.cleanCustomName")},
        {"cleanTag", tr("cleaner.form.setting.cleanTag")},
        {"cleanItemAge", tr("cleaner.form.setting.cleanItemAge")},
        {"cleanItemType", tr("cleaner.form.setting.cleanItemType")}
    };
    for (auto& [id, text] : toggles) fm.appendToggle(id, text, false);

    fm.sendTo(player, [=](Player& p, const ll::form::CustomFormResult& rst, ll::form::FormCancelReason res) {
        if (!rst) return;

        bool cleanTamed = std::get<uint64_t>(rst->at("cleanTamed"));
        bool cleanTrusting = std::get<uint64_t>(rst->at("cleanTrusting"));
        bool cleanCustomName = std::get<uint64_t>(rst->at("cleanCustomName"));
        bool cleanTag = std::get<uint64_t>(rst->at("cleanTag"));
        bool cleanItemAge = std::get<uint64_t>(rst->at("cleanItemAge"));
        bool cleanItemType = std::get<uint64_t>(rst->at("cleanItemType"));

        ConfirmForm(p, isOnline, TypeName,
            !cleanTamed, !cleanTrusting, !cleanCustomName,
            !cleanTag, !cleanItemAge, !cleanItemType);
    });
}

void OnlineEntityForm(Player& player) {
    auto fm = ll::form::SimpleForm(tr("cleaner.form.runtime.title"), tr("cleaner.form.runtime.content"));
    for (auto& id : getAllEntityTypes()) {
        fm.appendButton(tr("cleaner.form.common.button", {
            tr(typeNameToLocString(id)), std::to_string(getEntityCountByType(id))
        }), [id](Player& p) { SettingCleanForm(p, true, id); });
    }
    fm.sendTo(player);
}

void OfflineEntityForm(Player& player) {
    Cleaner::Entry::getInstance().getSelf().getLogger().info("preload");
    if (notCooldown() && !OfflineEntity.isUpdating) {
        updateUnloadEntityMap({}, false);
    }

    auto fm = ll::form::SimpleForm(tr("cleaner.form.unload.title"), tr("cleaner.form.unload.content"));
    if (OfflineEntity.lastUpdate != std::chrono::system_clock::time_point{}) {
        fm.appendLabel(tr("cleaner.form.unload.updated", {std::format("{:%Y-%m-%d %H:%M}", OfflineEntity.lastUpdate)}));
    }

    if (OfflineEntity.isUpdating.load()) {
        fm.appendLabel(tr("cleaner.form.unload.updating"));
        {
            std::lock_guard lock(OfflineEntity.offlineEntitiesMutex);
            OfflineEntity.waitedPlayers.insert(player.getOrCreateUniqueID());
        }
        fm.appendButton(tr("cleaner.form.unload.refresh"), [](Player& p) { OfflineEntityForm(p); });
    } else {
        fm.appendButton(tr("cleaner.form.unload.refresh"), [](Player& p) {
            OfflineEntity.requireUpdate.store(true);
            if (notCooldown()) {
                p.sendMessage(tr("cleaner.unload.updated.warn"));
                return;
            }
            if (OfflineEntity.isUpdating) {
                p.sendMessage(tr("cleaner.unload.update.warn"));
                return;
            }
            updateUnloadEntityMap({}, false);
        });
    }

    {
        std::lock_guard lock(OfflineEntity.offlineEntitiesMutex);
        for (auto& [type, ents] : OfflineEntity.offlineEntities) {
            fm.appendButton(tr("cleaner.form.common.button", {
                tr(typeNameToLocString(type)), std::to_string(ents.size())
            }), [type](Player& p) { SettingCleanForm(p, false, type); });
        }
    }
    fm.sendTo(player);
}

void MainForm(Player* player) {
    auto fm = ll::form::SimpleForm(tr("cleaner.form.main.title"), tr("cleaner.form.main.context"));
    fm.appendButton(tr("cleaner.form.main.runtime"), [](Player& p) { OnlineEntityForm(p); });
    fm.appendButton(tr("cleaner.form.main.unload"), [](Player& p) { OfflineEntityForm(p); });
    fm.sendTo(*player);
}
