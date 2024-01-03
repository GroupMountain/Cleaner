#pragma once
#include "Plugin.h"
#include <ll/api/command/DynamicCommand.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/ListenerBase.h>
#include <ll/api/event/command/SetupCommandEvent.h>
#include <ll/api/plugin/NativePlugin.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/service/Bedrock.h>
#include <ll/api/schedule/Scheduler.h>
#include <ll/api/schedule/Task.h>
#include "include_mcapi.h"

extern ll::Logger logger;
extern void RegisterCommand(CommandRegistry &registry);

namespace Cleaner {
    extern void CleanSchedule(int seconds, int notice_time);
}