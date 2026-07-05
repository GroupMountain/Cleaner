#include "Cleaner.h"
#include "Global.h"
#include "Mod.h"

#include "ll/api/base/Containers.h"
#include "ll/api/service/Bedrock.h"
#include "mc/common/Globals.h"
#include "mc/world/level/Level.h"
#include <gmlib/mc/world/Level.h>
#include <gmlib/mc/world/actor/UnloadedActor.h>
#include <ll/api/form/CustomForm.h>
#include <ll/api/form/ModalForm.h>
#include <ll/api/form/SimpleForm.h>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

using UnloadEntityMap = std::unordered_map<std::string, std::vector<gmlib::UnloadedActor>>;

struct {
    std::chrono::system_clock::time_point lastUpdate{};
    std::atomic_bool                      isUpdating{false};
    UnloadEntityMap                       offlineEntities{};
    ll::SmallDenseSet<ActorUniqueID>      waitedPlayers{};
    std::atomic_bool                      requireUpdate{false};

    std::mutex                            offlineEntitiesMutex; // ✅ 新增
} OfflineEntity;

auto typeNameToLocString(std::string const& typeName) {
    return EntityTypeToLocString(
        EntityTypeFromString(typeName),
        ActorTypeNamespaceRules::ReturnWithoutNamespace
    );
}

auto getAllEntityTypes() {
    return std::ranges::to<ll::SmallDenseSet<std::string>>(
        ll::service::getLevel()->getRuntimeActorList()
        | std::views::transform(&Actor::getTypeName)
    );
}

auto getEntityCountByType(std::string const& typeName) {
    return std::ranges::count_if(
        ll::service::getLevel()->getRuntimeActorList(),
        [&](const Actor* a) { return a && a->getTypeName() == typeName; }
    );
}

void ConfirmForm(Player& player, bool isOnline, const std::string& TypeName, bool needCondition) {
    auto fm = ll::form::ModalForm(
        tr("cleaner.form.confirm.title"),
        tr(isOnline&&needCondition?"cleaner.form.confirm.important"
            :"cleaner.form.confirm.warning",{
            tr(typeNameToLocString(TypeName)),
            TypeName,
            std::to_string(
            isOnline
                ? getEntityCountByType(TypeName)
                : OfflineEntity.offlineEntities[TypeName].size()
        )
        }),
        tr("cleaner.form.confirm.confirm"),
        tr("cleaner.form.confirm.cancel")
    );

    fm.sendTo(player, [isOnline, TypeName, needCondition](
        const Player& player,
        ll::form::ModalFormResult rst,
        ll::form::FormCancelReason res
    ) {
        if (res) return;

        if (!rst || rst.value() != ll::form::ModalFormSelectedButton::Upper)
            return;

        if (isOnline) {
            for (auto en : ll::service::getLevel()->getRuntimeActorList()) {
                auto  actor     = (gmlib::GMActor*)en;

                if (en->getTypeName() == TypeName &&
                    (!needCondition || !Cleaner::shouldIgnore(actor))) {
                    en->despawn();
                }
            }
        } else {
            if (!OfflineEntity.isUpdating.exchange(true)) {
                std::thread([TypeName]() {
                    {
                        std::lock_guard<std::mutex> lock(
                            OfflineEntity.offlineEntitiesMutex
                        );

                        auto it = OfflineEntity.offlineEntities.find(TypeName);
                        if (it != OfflineEntity.offlineEntities.end()) {
                            for (auto& en : it->second) {
                                if (en.isValid()) en.remove();
                            }
                            OfflineEntity.offlineEntities.erase(it);
                        }
                    }

                    OfflineEntity.isUpdating.store(false);
                }).detach();
            } else {
                player.sendMessage(tr(""));
            }
        }
    });
}

void SettingCleanForm(Player& player, bool isOnline, const std::string& TypeName) {
    if (!isOnline) {
        ConfirmForm(player, isOnline, TypeName, false);
        return;
    }

    auto fm = ll::form::CustomForm(tr("cleaner.form.setting.title"));
    fm.appendLabel(
        tr("cleaner.form.setting.content",{
            tr(typeNameToLocString(TypeName)),
            TypeName,
            std::to_string(
            isOnline
                ? getEntityCountByType(TypeName)
                : OfflineEntity.offlineEntities[TypeName].size()
        )
        })
    );
    fm.appendToggle(
        "cleanIgnoreActor",
        tr("cleaner.form.setting.cleanIgnore"),
        false,
        tr("cleaner.form.setting.cleanIgnore.tip")
    );

    fm.sendTo(player, [TypeName, isOnline](
        Player& player,
        const ll::form::CustomFormResult& rst,
        ll::form::FormCancelReason res
    ) {
        if (!rst) return;

        bool cleanIgnoreActor =
            std::get<uint64>(rst->at("cleanIgnoreActor"));

        ConfirmForm(player, isOnline, TypeName, !cleanIgnoreActor);
    });
}

void OnlineEntityForm(Player& player) {
    auto fm = ll::form::SimpleForm(tr("cleaner.form.runtime.title"), tr("cleaner.form.runtime.content"));

    for (auto& entityID : getAllEntityTypes()) {
        fm.appendButton(
            typeNameToLocString(entityID),
            [entityID](Player& player) {
                SettingCleanForm(player, true, entityID);
            }
        );
    }

    fm.sendTo(player);
}

void updateUnloadEntityMap(const Player& player, bool notice = false) {
    if (OfflineEntity.lastUpdate.time_since_epoch().count() == 0)
        OfflineEntity.requireUpdate.store(true);

    if (OfflineEntity.lastUpdate >=
        std::chrono::system_clock::now() - std::chrono::minutes(10)) {

        if (notice) player.sendMessage(tr("cleaner.unload.updated.warn"));

        return;
    }

    if (!OfflineEntity.isUpdating.exchange(true) &&
        OfflineEntity.requireUpdate) {

        std::thread([]() {
            UnloadEntityMap offlineEntityMap{};


            gmlib::UnloadedActor::foreachUnloadedActor(
                [&](gmlib::UnloadedActor& actor) -> bool {
                    offlineEntityMap[actor.getTypeName()]
                        .emplace_back(std::move(actor));
                    return true;
                }
            );


            {
                std::lock_guard<std::mutex> lock(
                    OfflineEntity.offlineEntitiesMutex
                );

                for (auto& playerID : OfflineEntity.waitedPlayers) {
                    if (auto pl =
                            ll::service::getLevel()->getPlayer(playerID))
                        pl->sendMessage(tr("cleaner.unload.updated.msg"));
                }

                OfflineEntity.offlineEntities.swap(offlineEntityMap);

            }

            OfflineEntity.lastUpdate = std::chrono::system_clock::now();
            OfflineEntity.isUpdating.store(false);
        }).detach();

    } else if (notice) {
        player.sendMessage(tr("cleaner.unload.update.warn"));
    }
}

void OfflineEntityForm(Player& player) {
    Cleaner::Entry::getInstance().getSelf().getLogger().info("preload");
    updateUnloadEntityMap(player);

    auto fm = ll::form::SimpleForm(tr("cleaner.form.unload.title"), tr("cleaner.form.unload.content"));

    if (OfflineEntity.lastUpdate !=
        std::chrono::system_clock::time_point{}) {

        fm.appendLabel(
            tr("cleaner.form.unload.updated",{std::format("{:%Y-%m-%d %H:%M}", OfflineEntity.lastUpdate)})
        );
    }

    if (OfflineEntity.isUpdating.load()) {
        fm.appendLabel(
            tr("cleaner.form.unload.updating")
        );

        {
            std::lock_guard<std::mutex> lock(
                OfflineEntity.offlineEntitiesMutex
            );
            OfflineEntity.waitedPlayers.insert(
                player.getOrCreateUniqueID()
            );
        }

        fm.appendButton(tr("cleaner.form.unload.refresh"), [](Player& player) {
            OfflineEntityForm(player);
        });
    } else {
        fm.appendButton(tr("cleaner.form.unload.refresh"), [](Player& player) {
            OfflineEntity.requireUpdate.store(true);
            updateUnloadEntityMap(player, true);
        });
    }

    {
        std::lock_guard<std::mutex> lock(
            OfflineEntity.offlineEntitiesMutex
        );

        for (auto& entity : OfflineEntity.offlineEntities) {
            fm.appendButton(
                typeNameToLocString(entity.first),
                [typeName = entity.first](Player& player) {
                    SettingCleanForm(player, false, typeName);
                }
            );
        }
    }

    fm.sendTo(player);
}

void MainForm(Player* player) {
    auto fm = ll::form::SimpleForm(tr("cleaner.form.main.title"), tr("cleaner.form.main.context"));

    fm.appendButton(tr("cleaner.form.main.runtime"), [](Player& player) {
        OnlineEntityForm(player);
    });

    fm.appendButton(tr("cleaner.form.main.unload"), [](Player& player) {
        OfflineEntityForm(player);
    });

    fm.sendTo(*player);
}
