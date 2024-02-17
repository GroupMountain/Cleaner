#pragma once
#include "Plugin.h"
#include <include_all.h>
#include <regex>

#define S(x) std::to_string(x)

extern ll::Logger logger;

extern GMLIB::Files::JsonConfig*     Config;
extern GMLIB::Files::I18n::JsonI18n* Language;

extern void RegisterCommands();
extern void initPlugin();

namespace ConfigFile {
extern int  mItemDespawnTicks;
extern bool mConsoleLog;
extern bool mAnnounce;
extern bool mSendToast;
} // namespace ConfigFile

namespace Helper {
extern void broadcastMessage(std::string_view msg);
extern void broadcastToast(std::string_view msg);
} // namespace Helper

extern std::string tr(std::string key, std::vector<std::string> data = {});