#pragma once
#include "Plugin.h"
#include "include_all.h"
#include <GMLIB/Server/ActorAPI.h>
#include <GMLIB/Server/LevelAPI.h>

extern ll::Logger logger;
extern void       RegisterCommands();
extern void       UnregisterCommands();

extern int item_despawn_time;

namespace Cleaner {

static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mAutoCleanTask;
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mCheckCleanTask;
extern void                                                               CleanTask(int time, int announce_time);
extern void                                                               AutoCleanTask(int seconds);
extern void CheckCleanTask(int max_entities, float min_tps);
extern void ReloadCleaner();
extern void ListenEvents();

} // namespace Cleaner