#pragma once
#include "Plugin.h"
#include "include_all.h"

extern ll::Logger logger;
extern void RegisterCommands();
extern void UnregisterCommands();

namespace Cleaner {
    extern void CleanTask();
}