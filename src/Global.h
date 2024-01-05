#pragma once
#include "Plugin.h"
#include "include_all.h"

extern ll::Logger logger;
extern void RegisterCommands();
extern void UnregisterCommands();
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::GameTimeClock>> auto_clean_task;

namespace Cleaner {
    extern void CleanTask();
    extern void AutoCleanTask();
}