#include "Cleaner.h"

namespace Cleaner {

void loadCleaner() {
    if (Config->getValue<bool>({"ScheduleClean", "Enabled"}, false)) {
        Cleaner::AutoCleanTask(Config->getValue<int>({"ScheduleClean", "CleanInterval"}, 3000));
    }
    if (Config->getValue<bool>({"AutoCleanCount", "Enabled"}, false)) {
        Cleaner::CleanTaskCount(Config->getValue<int>({"AutoCleanCount", "TriggerCount"}, 900));
    }
    if (Config->getValue<bool>({"AutoCleanTPS", "Enabled"}, false)) {
        Cleaner::CleanTaskTPS(Config->getValue<float>({"AutoCleanTPS", "TriggerTPS"}, 15.0f));
    }
    ConfigFile::mItemDespawnTicks = Config->getValue<int>({"ItemDespawn", "DespawnTime"}, 6000);
    ConfigFile::mAnnounce         = Config->getValue<bool>({"Basic", "SendBroadcast"}, true);
    ConfigFile::mConsoleLog       = Config->getValue<bool>({"Basic", "ConsoleLog"}, true);
    ConfigFile::mSendToast        = Config->getValue<bool>({"Basic", "SendToast"}, true);
    ConfigFile::mIgnoreTags       = Config->getValue<std::vector<std::string>>({"IgnoreTags"}, {});
}

void unloadCleaner() { stopAllTasks(); }

void reloadCleaner() {
    unloadCleaner();
    Config->reload();
    std::string languageCode = Config->getValue<std::string>({"Basic", "Language"}, "en_US");
    Language->reloadAllLanguages();
    Language->chooseLanguage(languageCode);
    loadCleaner();
}

} // namespace Cleaner