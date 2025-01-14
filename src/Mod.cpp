#include "Mod.h"
#include "Features/Cleaner.h"
#include "Global.h"
#include "Language.h"

namespace Cleaner {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() { return true; }

bool Entry::enable() {
    mConfig.emplace();
    ll::config::loadConfig(*mConfig, getSelf().getConfigDir() / "config.json");
    saveConfig();
    I18nAPI::updateOrCreateLanguageFile(getSelf().getLangDir(), "en_US", en_US);
    I18nAPI::updateOrCreateLanguageFile(getSelf().getLangDir(), "zh_CN", zh_CN);
    I18nAPI::loadLanguagesFromDirectory(getSelf().getLangDir());
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
    return I18nAPI::get(key, params, Cleaner::Entry::getInstance().getConfig().language);
}
