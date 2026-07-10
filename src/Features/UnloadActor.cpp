#include "Cleaner.h"
#include "Global.h"
#include "Mod.h"

#include "ll/api/base/Containers.h"
#include "ll/api/service/Bedrock.h"
#include "mc/common/Globals.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/level/Level.h"
#include <gmlib/mc/world/actor/Actor.h>
#include <gmlib/mc/world/actor/UnloadedActor.h>
#include <ll/api/form/CustomForm.h>
#include <ll/api/form/ModalForm.h>
#include <ll/api/form/SimpleForm.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using UnloadEntityMap = std::unordered_map<std::string, std::vector<gmlib::UnloadedActor>>;

struct OfflineEntityData {
    std::chrono::system_clock::time_point lastUpdate{};
    std::atomic_bool                      isUpdating{false};
    UnloadEntityMap                       offlineEntities{};
    ll::SmallDenseSet<ActorUniqueID>      waitedPlayers{};
    std::atomic_bool                      requireUpdate{false};
    std::mutex                            offlineEntitiesMutex;
} OfflineEntity;





void updateUnloadEntityMap(std::vector<std::string> CleanList, bool skipCheck) {
    OfflineEntity.isUpdating.store(true);
    if ((OfflineEntity.requireUpdate || skipCheck)) {
        std::thread([CleanList]() {
            UnloadEntityMap offlineEntityMap{};
            gmlib::UnloadedActor::foreachUnloadedActor(
                [&](gmlib::UnloadedActor& actor) -> bool {
                    if (!actor.getTypeName().empty() && actor.getTypeName() != "minecraft:player") {
                        if (std::ranges::find_if(CleanList, [&](auto& x) { return x == actor.getTypeName(); }) != CleanList.end()) {
                            actor.remove();
                            return true;
                        }
                        offlineEntityMap[actor.getTypeName()].emplace_back(std::move(actor));
                    }
                    return true;
                }
            );
            
            {
                std::lock_guard<std::mutex> lock(OfflineEntity.offlineEntitiesMutex);
                for (auto& playerID : OfflineEntity.waitedPlayers) {
                    if (auto pl = ll::service::getLevel()->getPlayer(playerID)) {
                        pl->sendMessage(tr("cleaner.unload.updated.msg"));
                    }
                }
                OfflineEntity.offlineEntities.swap(offlineEntityMap);
            }
            
            OfflineEntity.lastUpdate = std::chrono::system_clock::now();
            OfflineEntity.isUpdating.store(false);
        }).detach();
    }
}
