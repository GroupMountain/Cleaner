#pragma once
#include "Mod.h"
#include <include_all.h>
#include <regex>

#define S(x) std::to_string(x)

extern void RegisterCommands();

namespace Helper {
extern void broadcastMessage(std::string_view msg);
extern void broadcastToast(std::string_view msg);
} // namespace Helper

extern std::string tr(std::string const& key, std::vector<std::string> const& params = {});