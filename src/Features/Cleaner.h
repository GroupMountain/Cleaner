#pragma once
#include "Global.h"

namespace Cleaner {

extern int ExecuteClean();
extern int CountEntities();

extern void AutoCleanTask(int seconds);
extern void CheckCleanTask(int max_entities, float min_tps);
extern void ListenEvents();
extern void stopAllTasks();

} // namespace Cleaner