#pragma once
#include "Plugin.h"
#include <include_all.h>

#define S(x) std::to_string(x)

extern ll::Logger                logger;
extern GMLIB::Files::JsonConfig* Config;

extern void RegisterCommands();
extern void initPlugin();

extern int item_despawn_time;

namespace Cleaner {

static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mAutoCleanTask;
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mCheckCleanTask;

extern void loadCleaner();
extern void unloadCleaner();
extern void reloadCleaner();
extern void CleanTask();

} // namespace Cleaner

extern std::string tr(std::string key, std::vector<std::string> data = {});