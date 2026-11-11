#pragma once
#include "Global.h"

#include <gmlib/mc/world/actor/UnloadedActor.h>
#include <mc/deps/nbt/CompoundTag.h>
#include <ll/api/base/Containers.h>

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

extern void MainForm(Player* player);
extern void updateUnloadEntityMap(std::vector<std::string> CleanList = {}, bool skipCheck = false);

using namespace ll::chrono_literals;


struct UnloadedEntityBrief {
    ActorUniqueID            id{};
    int64_t                  storageKey{};
    std::vector<std::string> tags{};
    std::string              itemName{};
    int16_t                  itemAge{0};
    int8_t                   arrowAux{0};
    bool                     isTamed{false};
    bool                     isTrusting{false};
    bool                     customNameVisible{false};
    bool                     hasItemAge{false};
    bool                     hasItemName{false};
};

using BriefMap = std::unordered_map<std::string, std::vector<UnloadedEntityBrief>>;

struct OfflineEntityData {
    std::chrono::system_clock::time_point lastUpdate{};
    std::atomic_bool                      isUpdating{false};
    BriefMap                              offlineEntities{};
    ll::SmallDenseSet<ActorUniqueID>      waitedPlayers{};
    std::atomic_bool                      requireUpdate{false};
    std::mutex                            offlineEntitiesMutex;
};
extern OfflineEntityData OfflineEntity;

extern void cleanUnloadedEntitiesOfType(
    std::string const& typeName,
    bool               checkTamed,
    bool               checkTrusting,
    bool               checkCustomName,
    bool               checkTag,
    bool               checkItemAge,
    bool               checkItemType,
    bool               checkEffectArrow
);

namespace Cleaner {

extern int ExecuteClean();
extern int CountEntities();

extern void AutoCleanTask(int seconds);
extern void CleanTaskCount(int max_entities);
extern void CleanTaskTPS(float min_tps);
extern void ListenEvents();
extern void stopAllTasks();
extern void CleanTask();
extern void reloadCleaner();
extern void loadCleaner();
extern void unloadCleaner();

extern bool isMatch(const std::string& A, const std::string& B);

} // namespace Cleaner

namespace VoteClean {

extern void voteCommandExecute(Player* pl);

}

namespace UnloadActorClean {

extern void cleanUnloadActor();

}