#pragma once
#include "Plugin.h"
#include "include_all.h"

extern ll::Logger logger;
extern void RegisterCommands();
extern void UnregisterCommands();
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::GameTimeClock>> auto_clean_task;
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::GameTimeClock>> check_clean_task;
extern int item_despawn_time;

namespace Cleaner {
    extern void CleanTask(int time, int announce_time);
    extern void AutoCleanTask(int seconds);
    extern void CheckCleanTask(int max_entities, float min_tps);
}

namespace TPS {
    extern void CaculateTPS();
    extern float getCurrentTps();
    extern float getAverageTps();
    extern float getMspt();
}