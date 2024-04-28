#include "Features/Cleaner.h"
#include "Global.h"
#include "Language.h"

ll::Logger logger("Cleaner");

namespace Cleaner {

static std::unique_ptr<Entry> instance;

Entry& Entry::getInstance() { return *instance; }

bool Entry::load() { return true; }

bool Entry::enable() {
    mConfig.emplace();
    mScheduler.emplace();
    ll::config::loadConfig(*mConfig, getSelf().getConfigDir() / "config.json");
    saveConfig();
    I18nAPI::updateOrCreateLanguageFile(getSelf().getLangDir(), "en_US", en_US);
    I18nAPI::updateOrCreateLanguageFile(getSelf().getLangDir(), "zh_CN", zh_CN);
    I18nAPI::loadLanguagesFromDirectory(getSelf().getLangDir());
    Cleaner::ListenEvents();
    RegisterCommands();
    Cleaner::loadCleaner();
    logger.info("Cleaner Loaded!");
    logger.info("Author: GroupMountain");
    logger.info("Repository: https://github.com/GroupMountain/Cleaner");
    return true;
}

bool Entry::disable() {
    mConfig.reset();
    Cleaner::unloadCleaner();
    mScheduler.reset();
    return true;
}

Config& Entry::getConfig() { return mConfig.value(); }

void Entry::saveConfig() { ll::config::saveConfig(*mConfig, getSelf().getConfigDir() / "config.json"); }

ServerTimeScheduler& Entry::getScheduler() { return mScheduler.value(); }

} // namespace Cleaner

LL_REGISTER_PLUGIN(Cleaner::Entry, Cleaner::instance);