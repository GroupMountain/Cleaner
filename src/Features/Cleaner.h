#pragma once
#include "Global.h"

using namespace ll::schedule;
using namespace ll::chrono_literals;

namespace Cleaner {

extern ServerTimeScheduler scheduler;

extern int ExecuteClean();
extern int CountEntities();

extern void AutoCleanTask(int seconds);
extern void CheckCleanTask(int max_entities, float min_tps);
extern void ListenEvents();
extern void stopAllTasks();
extern void CleanTask();
extern void reloadCleaner();
extern void loadCleaner();
extern void unloadCleaner();

} // namespace Cleaner

namespace VoteClean {

extern void voteCommandExecute(Player* pl);

}