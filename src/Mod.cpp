#include "Mod.h"
#include "Features/Cleaner.h"
#include "Global.h"
#include "Language.h"
#include "ll/api/utils/ErrorUtils.h"
#include "gmlib/mc/locale/I18nAPI.h"
namespace Cleaner {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() { return true; }

bool Entry::enable() {
    mConfig.emplace();
    try {
        ll::config::loadConfig(*mConfig, getSelf().getConfigDir() / "config.json");
    } catch (...) {
        ll::error_utils::printCurrentException(getSelf().getLogger());
    }
    saveConfig();
    gmlib::I18nAPI::updateOrCreateLanguageFile(getSelf().getLangDir(), "en_US", en_US);
    gmlib::I18nAPI::updateOrCreateLanguageFile(getSelf().getLangDir(), "zh_CN", zh_CN);
    gmlib::I18nAPI::loadLanguagesFromDirectory(getSelf().getLangDir());
    Cleaner::ListenEvents();
    RegisterCommands();
    Cleaner::loadCleaner();
    getSelf().getLogger().info("Cleaner Loaded!");
    getSelf().getLogger().info("Author: GroupMountain");
    getSelf().getLogger().info("Repository: https://github.com/GroupMountain/Cleaner");
    return true;
}

bool Entry::disable() {
    mConfig.reset();
    Cleaner::unloadCleaner();
    return true;
}

Config& Entry::getConfig() { return mConfig.value(); }

void Entry::saveConfig() { ll::config::saveConfig(*mConfig, getSelf().getConfigDir() / "config.json"); }

} // namespace Cleaner

LL_REGISTER_MOD(Cleaner::Entry, Cleaner::Entry::getInstance());

std::string tr(std::string const& key, std::vector<std::string> const& params) {
    return gmlib::I18nAPI::get(key, params);
}
