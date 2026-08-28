#include "Cleaner.h"
#include "Global.h"
#include "Mod.h"

#include "ll/api/base/Containers.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/ui/form/CustomForm.h"
#include "mc/common/Globals.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/storage/DBStorage.h"
#include <gmlib/mc/world/CompoundTag.h>
#include <gmlib/mc/world/actor/UnloadedActor.h>
#include <ll/api/form/CustomForm.h>
#include <ll/api/form/ModalForm.h>
#include <ll/api/form/SimpleForm.h>
#include <mc/deps/vanilla_components/ActorTypeComponent.h>
#include <mc/world/actor/projectile/Arrow.h>
#include <mc/world/actor/provider/SynchedActorDataAccess.h>

#include <algorithm>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace std::ranges;
using namespace ll::ui;
extern struct OfflineEntityData {
    std::chrono::system_clock::time_point                              lastUpdate;
    std::atomic_bool                                                   isUpdating;
    std::unordered_map<std::string, std::vector<gmlib::UnloadedActor>> offlineEntities;
    ll::SmallDenseSet<ActorUniqueID>                                   waitedPlayers;
    std::atomic_bool                                                   requireUpdate;
    std::mutex                                                         offlineEntitiesMutex;
} OfflineEntity;

bool shouldClean(
    gmlib::GMActor* actor,
    bool            checkTamed,
    bool            checkTrusting,
    bool            checkCustomName,
    bool            checkTag,
    bool            checkItemAge,
    bool            checkItemType,
    bool            checkEffectArrow
) {
    auto& cfg = Cleaner::Entry::getInstance().getConfig();

    if (checkTamed && actor->isTame()) return false;
    if (checkTrusting && SynchedActorDataAccess::getActorFlag(actor->getEntityContext(), ActorFlags::Trusting))
        return false;
    if (checkCustomName && actor->getNameTag() != "") return false;
    if (checkTag) {
        if (actor->hasTag("cleaner:ignore") || any_of(cfg.IgnoreTags, [&](auto& t) { return actor->hasTag(t); }))
            return false;
    }
    if (checkEffectArrow && actor->mEntityContext->tryGetComponent<ActorTypeComponent>()->mType == ActorType::Arrow) {
        if (auto arrow = (Arrow*)actor) {
            if (arrow->getAuxValue() != 0) {
                return false;
            }
        }
    }

    if (actor->hasCategory(::ActorCategory::Item)) {
        auto itac = (ItemActor*)actor;
        if (checkItemAge && itac->age() <= cfg.CleanItem.ExistTicks) {
            return false;
        }
        auto itemType  = itac->item().getTypeName();
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

bool shouldCleanUnload(
    const gmlib::UnloadedActor& actor,
    bool                        checkTamed,
    bool                        checkTrusting,
    bool                        checkCustomName,
    bool                        checkTag,
    bool                        checkItemAge,
    bool                        checkItemType,
    bool                        checkEffectArrow
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
    if (checkEffectArrow && actor.getTypeName() == "minecraft:arrow") {
        auto& auxValue = nbt->at("auxValue");
        if (auxValue) {
            if (auto& aux = auxValue.get<ByteTag>()) {
                if (aux.data != 0) {
                    return false;
                }
            }
        }
    }

    if (checkTag && nbt->contains("Tags")) {
        auto& tags = nbt->at("Tags").get<ListTag>();
        for (auto& t : tags | views::filter([](auto& v) { return v->as<StringTag>().data() != nullptr; })) {
            auto tag = t->as<StringTag>().data();
            if (tag == "cleaner:ignore" || any_of(cfg.IgnoreTags, [&](auto& kt) { return tag == kt; })) return false;
        }
    }

    if (actor.getTypeName() == "minecraft:item") {
        if (checkItemAge && nbt->contains("Age") && nbt->at("Age").get<ShortTag>().data <= cfg.CleanItem.ExistTicks)
            return false;
        if (checkItemType && nbt->contains("Item")) {
            auto& itemNbt = nbt->at("Item").get<CompoundTag>();
            if (itemNbt.contains("Name")) {
                std::string itemType  = itemNbt.at("Name").get<StringTag>().data();
                auto        whitelist = cfg.CleanItem.Whitelist;
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

bool notCooldown() { return OfflineEntity.lastUpdate + std::chrono::minutes(10) >= std::chrono::system_clock::now(); }

auto typeNameToLocString(std::string const& typeName) {
    if (EntityTypeFromString(typeName) == ActorType::Undefined) {
        return typeName;
    }
    return EntityTypeToLocString(EntityTypeFromString(typeName), ActorTypeNamespaceRules::ReturnWithoutNamespace);
}

auto getAllEntityTypes() {
    std::unordered_map<std::string, std::size_t> cnt;
    for_each(ll::service::getLevel()->getRuntimeActorList() | std::views::transform(&Actor::getTypeName), [&](auto& n) {
        ++cnt[n];
    });
    auto ids = cnt | std::views::keys | std::ranges::to<std::vector>();
    sort(ids, [&](auto& a, auto& b) { return cnt[a] > cnt[b]; });
    return ids;
}


/*
* auto getAllEntityTypes() {
    return to<ll::SmallDenseSet<std::string>>(
        ll::service::getLevel()->getRuntimeActorList()
        | views::transform(&Actor::getTypeName)
    );
}
 */

auto getEntityCountByType(std::string_view typeName) {
    return count_if(ll::service::getLevel()->getRuntimeActorList(), [&](const Actor* a) {
        return a && a->getTypeName() == typeName;
    });
}

void ConfirmForm(
    Player&          player,
    bool             isOnline,
    std::string_view TypeName,
    bool             checkTamed,
    bool             checkTrusting,
    bool             checkCustomName,
    bool             checkTag,
    bool             checkItemAge,
    bool             checkItemType,
    bool             checkEffectArrow
) {
    std::string typeNameStr(TypeName);


    auto fm = ll::form::ModalForm(
        tr("cleaner.form.confirm.title"),
        tr("cleaner.form.confirm.warning",
           {tr(typeNameToLocString(typeNameStr)),
            typeNameStr,
            S(isOnline ? count_if(
                             ll::service::getLevel()->getRuntimeActorList(),
                             [&](const Actor* a) {
                                 return a && a->getTypeName() == typeNameStr
                                     && shouldClean(
                                            (gmlib::GMActor*)a,
                                            checkTamed,
                                            checkTrusting,
                                            checkCustomName,
                                            checkTag,
                                            checkItemAge,
                                            checkItemType,
                                            checkEffectArrow
                                     );
                             }
                         )
              : OfflineEntity.offlineEntities.contains(typeNameStr)
                  ? OfflineEntity.offlineEntities.at(typeNameStr).size()
                  : 0)}),
        tr("cleaner.form.confirm.confirm"),
        tr("cleaner.form.confirm.cancel")
    );

    fm.sendTo(player, [=](const Player& p, ll::form::ModalFormResult rst, ll::form::FormCancelReason res) {
        if (res || !rst || rst.value() != ll::form::ModalFormSelectedButton::Upper) return;
        if (isOnline) {
            for (auto* en : ll::service::getLevel()->getRuntimeActorList()) {
                if (en->getTypeName() == typeNameStr
                    && shouldClean(
                        (gmlib::GMActor*)en,
                        checkTamed,
                        checkTrusting,
                        checkCustomName,
                        checkTag,
                        checkItemAge,
                        checkItemType,
                        checkEffectArrow
                    )) {
                    en->despawn();
                }
            }
        } else {
            std::thread([=]() {
                {
                    std::lock_guard<std::mutex>       lock(OfflineEntity.offlineEntitiesMutex);
                    std::vector<gmlib::UnloadedActor> unloadActors;
                    auto                              it = OfflineEntity.offlineEntities.find(typeNameStr);
                    if (it != OfflineEntity.offlineEntities.end()) {
                        for (auto& en : it->second) {
                            if (en.isValid()
                                && (shouldCleanUnload(
                                    en,
                                    checkTamed,
                                    checkTrusting,
                                    checkCustomName,
                                    checkTag,
                                    checkItemAge,
                                    checkItemType,
                                    checkEffectArrow
                                )))
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

void SettingCleanForm(Player& player, bool isOnline, std::string_view TypeName) {
    std::string typeNameStr(TypeName);

    auto fm = ll::form::CustomForm(tr("cleaner.form.setting.title"));
    fm.appendLabel(
        tr("cleaner.form.setting.content",
           {tr(typeNameToLocString(typeNameStr)),
            typeNameStr,
            S(isOnline ? getEntityCountByType(typeNameStr)
                       : (OfflineEntity.offlineEntities.contains(typeNameStr)
                              ? OfflineEntity.offlineEntities.at(typeNameStr).size()
                              : 0))})
    );

    const auto toggles = [&] {
        std::vector<std::pair<const char*, std::string>> v = {
            {"cleanTamed",      tr("cleaner.form.setting.cleanTamed")     },
            {"cleanTrusting",   tr("cleaner.form.setting.cleanTrusting")  },
            {"cleanCustomName", tr("cleaner.form.setting.cleanCustomName")},
            {"cleanTag",        tr("cleaner.form.setting.cleanTag")       }
        };
        if (typeNameStr == "minecraft:item") {
            v.insert(
                v.end(),
                {
                    {"cleanItemAge",  tr("cleaner.form.setting.cleanItemAge") },
                    {"cleanItemType", tr("cleaner.form.setting.cleanItemType")}
            }
            );
        }
        if (typeNameStr == "minecraft:arrow") {
            v.insert(
                v.end(),
                {
                    {"cleanEffectArrow", tr("cleaner.form.setting.cleanEffectArrow")}
            }
            );
        }
        return v;
    }();

    for (auto& [id, text] : toggles) fm.appendToggle(id, text, false);

    fm.sendTo(player, [=](Player& p, const ll::form::CustomFormResult& rst, ll::form::FormCancelReason res) {
        if (res) {
            if (res == ModalFormCancelReason::UserBusy) {
                SettingCleanForm(p, isOnline, typeNameStr);
            }
        }
        if (!rst) return;

        bool cleanTamed      = std::get<uint64_t>(rst->at("cleanTamed"));
        bool cleanTrusting   = std::get<uint64_t>(rst->at("cleanTrusting"));
        bool cleanCustomName = std::get<uint64_t>(rst->at("cleanCustomName"));
        bool cleanTag        = std::get<uint64_t>(rst->at("cleanTag"));
        bool cleanItemAge    = typeNameStr == "minecraft:item" ? std::get<uint64_t>(rst->at("cleanItemAge")) : false;
        bool cleanItemType   = typeNameStr == "minecraft:item" ? std::get<uint64_t>(rst->at("cleanItemType")) : false;
        bool cleanEffectArrow =
            typeNameStr == "minecraft:arrow" ? std::get<uint64_t>(rst->at("cleanEffectArrow")) : false;
        ConfirmForm(
            p,
            isOnline,
            typeNameStr,
            !cleanTamed,
            !cleanTrusting,
            !cleanCustomName,
            !cleanTag,
            !cleanItemAge,
            !cleanItemType,
            !cleanEffectArrow
        );
    });
}

void OnlineEntityForm(Player& player) {
    ObservableString keyword{"", {.clientWritable = true}};

    struct ButtonState {
        std::string                        type;
        std::shared_ptr<ObservableBoolean> visible;
    };

    auto entityTypes = getAllEntityTypes();
    auto statesPtr   = std::make_shared<std::vector<ButtonState>>();
    statesPtr->reserve(entityTypes.size());
    for (auto const& t : entityTypes) {
        statesPtr->push_back(ButtonState{t, std::make_shared<ObservableBoolean>(true)});
    }

    auto const subscription = keyword.subscribe([statesPtr](std::string const& kw) {
        std::string lowerKw = kw;
        std::transform(lowerKw.begin(), lowerKw.end(), lowerKw.begin(), ::tolower);
        for (auto& bs : *statesPtr) {
            if (lowerKw.empty()) {
                bs.visible->setData(true);
                continue;
            }
            std::string lowerTitle  = bs.type;
            std::string lowerTitle1 = tr(typeNameToLocString(bs.type));
            std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
            std::transform(lowerTitle1.begin(), lowerTitle1.end(), lowerTitle1.begin(), ::tolower);
            bs.visible->setData(
                lowerTitle.find(lowerKw) != std::string::npos || lowerTitle1.find(lowerKw) != std::string::npos
            );
        }
    });

    auto formPtr = std::make_shared<CustomForm>(player, tr("cleaner.form.runtime.title"));

    formPtr->label(tr("cleaner.form.runtime.content"));
    formPtr->spacer();
    formPtr->textField(tr("cleaner.form.common.search"), keyword);
    formPtr->spacer();


    for (auto& bs : *statesPtr) {
        std::string type = bs.type;
        formPtr->button(
            tr("cleaner.form.common.button", {tr(typeNameToLocString(bs.type)), S(getEntityCountByType(bs.type))}),
            [formPtr, playerUuid = player.getUuid(), type]() {
                // 第一步：关闭当前表单
                formPtr->close();

                // 第二步：打开新表单（检查玩家是否在线）
                if (auto* pl = ll::service::getLevel()->getPlayer(playerUuid)) SettingCleanForm(*pl, true, type);
            },
            {.visible = *bs.visible}
        );
    }

    if (!formPtr->show()) {
        keyword.unsubscribe(subscription);
    }
}


void OfflineEntityForm(Player& player) {
    if (notCooldown() && !OfflineEntity.isUpdating) {
        updateUnloadEntityMap({}, false);
    }
    ObservableString keyword{"", {.clientWritable = true}};

    struct ButtonStateUnload {
        std::string                        type;
        std::shared_ptr<ObservableBoolean> visible;
        size_t                             count;
    };

    auto entityTypes = getAllEntityTypes();
    auto statesPtr   = std::make_shared<std::vector<ButtonStateUnload>>();
    statesPtr->reserve(entityTypes.size());


    auto const subscription = keyword.subscribe([statesPtr](std::string const& kw) {
        std::string lowerKw = kw;
        std::transform(lowerKw.begin(), lowerKw.end(), lowerKw.begin(), ::tolower);
        for (auto& bs : *statesPtr) {
            if (lowerKw.empty()) {
                bs.visible->setData(true);
                continue;
            }
            std::string lowerTitle  = bs.type;
            std::string lowerTitle1 = tr(typeNameToLocString(bs.type));
            std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
            std::transform(lowerTitle1.begin(), lowerTitle1.end(), lowerTitle1.begin(), ::tolower);
            bs.visible->setData(
                lowerTitle.find(lowerKw) != std::string::npos || lowerTitle1.find(lowerKw) != std::string::npos
            );
        }
    });

    auto form = std::make_shared<CustomForm>(player, tr("cleaner.form.unload.title"));
    auto uuid = player.getUuid();
    form->label(tr("cleaner.form.unload.content"));
    if (OfflineEntity.lastUpdate != std::chrono::system_clock::time_point{}) {
        form->label(tr("cleaner.form.unload.updated", {std::format("{:%Y-%m-%d %H:%M}", OfflineEntity.lastUpdate)}));
    }
    form->spacer();
    form->textField(tr("cleaner.form.common.search"), keyword);
    form->spacer();

    if (OfflineEntity.isUpdating.load()) {
        form->label(tr("cleaner.form.unload.updating"));
        {
            std::lock_guard lock(OfflineEntity.offlineEntitiesMutex);
            OfflineEntity.waitedPlayers.insert(player.getOrCreateUniqueID());
        }
        form->button(tr("cleaner.form.unload.refresh"), [uuid, form]() {
            form->close();
            if (auto* pl = ll::service::getLevel()->getPlayer(uuid)) OfflineEntityForm(*pl);
        });
    } else {
        form->button(tr("cleaner.form.unload.refresh"), [uuid, form]() {
            form->close();
            if (auto* pl = ll::service::getLevel()->getPlayer(uuid)) {
                if (notCooldown()) {
                    pl->sendMessage(tr("cleaner.unload.updated.warn"));
                    return;
                }
                if (OfflineEntity.isUpdating) {
                    pl->sendMessage(tr("cleaner.unload.update.warn"));
                    return;
                }
                pl->sendMessage(tr("cleaner.form.unload.update"));
                OfflineEntity.requireUpdate.store(true);
                {
                    std::lock_guard lock(OfflineEntity.offlineEntitiesMutex);
                    OfflineEntity.waitedPlayers.insert(pl->getOrCreateUniqueID());
                }
                updateUnloadEntityMap({}, false);
            }
        });
    }

    {
        std::lock_guard                             lock(OfflineEntity.offlineEntitiesMutex);
        const auto&                                 entities = OfflineEntity.offlineEntities; // unordered_map
        std::vector<std::pair<std::string, size_t>> counts;
        counts.reserve(entities.size());
        for (const auto& [name, vec] : entities) {
            counts.emplace_back(name, vec.size());
        }
        std::ranges::sort(counts, [](const auto& a, const auto& b) { return a.second > b.second; });
        for (auto const& t : counts) {
            statesPtr->push_back(
                ButtonStateUnload{
                    t.first,
                    std::make_shared<ObservableBoolean>(true),
                    t.second,
                }
            );
        }
        for (auto& bs : *statesPtr) {
            form->button(
                tr("cleaner.form.common.button", {tr(typeNameToLocString(bs.type)), S(bs.count)}),
                [title = bs.type, uuid, form]() {
                    form->close();
                    if (auto* pl = ll::service::getLevel()->getPlayer(uuid)) SettingCleanForm(*pl, false, title);
                },
                {.visible = *bs.visible}
            );
        }
    }
    if (const auto started = form->show(); !started) keyword.unsubscribe(subscription);
}

void MainForm(Player* player) {
    auto fm = ll::form::SimpleForm(tr("cleaner.form.main.title"), tr("cleaner.form.main.context"));
    fm.appendButton(tr("cleaner.form.main.runtime"), [](Player& p) { OnlineEntityForm(p); });
    fm.appendButton(tr("cleaner.form.main.unload"), [](Player& p) { OfflineEntityForm(p); });
    fm.sendTo(*player);
}
