#pragma once
#include "Global.h"

using namespace ll::schedule;
using namespace ll::chrono_literals;

namespace Cleaner {

extern ServerTimeScheduler scheduler;

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

extern bool isMatch(std::string& A, std::string& B);

} // namespace Cleaner

namespace VoteClean {

extern void voteCommandExecute(Player* pl);

}