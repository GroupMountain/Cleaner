#pragma once
#include "Config.h"
#include "Plugin.h"
#include "include_all.h"
#include <GMLIB/Server/ActorAPI.h>
#include <GMLIB/Server/LevelAPI.h>
#include <GMLIB/Files/JsonConfig.h>
#include <GMLIB/Files/JsonLanguage.h>
#define S(x) std::to_string(x)

extern ll::Logger logger;
extern void       RegisterCommands();
extern void       UnregisterCommands();

extern GMLIB_JsonConfig* Config;
extern GMLIB_JsonLanguage* Language; 

extern int item_despawn_time;

namespace Cleaner {

static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mAutoCleanTask;
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mCheckCleanTask;
extern void loadConfig();
extern void loadCleaner();
extern void unloadCleaner();
extern void reloadCleaner();
extern void CleanTask(int time, int announce_time);

} // namespace Cleaner