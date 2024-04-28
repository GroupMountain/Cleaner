#pragma once
#include "Plugin.h"
#include <include_all.h>
#include <regex>

#define S(x) std::to_string(x)

extern ll::Logger logger;

extern void RegisterCommands();

namespace Helper {
extern void broadcastMessage(std::string_view msg);
extern void broadcastToast(std::string_view msg);
} // namespace Helper