#include "Cleaner.h"

namespace Cleaner {

void loadCleaner() {
    if (Config->getValue<bool>({"ScheduleClean", "Enabled"}, false)) {
        Cleaner::AutoCleanTask(Config->getValue<int>({"ScheduleClean", "CleanInterval"}, 3000));
    }
    if (Config->getValue<bool>({"AutoCleanCount", "Enabled"}, false)
        || Config->getValue<bool>({"AutoCleanTPS", "Enabled"}, false)) {
        Cleaner::CheckCleanTask(
            Config->getValue<int>({"AutoCleanCount", "TriggerCount"}, 900),
            Config->getValue<int>({"AutoCleanTPS", "TriggerTPS"}, 15)
        );
    }
    ConfigFile::mItemDespawnTicks = Config->getValue<int>({"ItemDespawn", "DespawnTime"}, 6000);
    ConfigFile::mAnnounce         = Config->getValue<bool>({"Basic", "SendBroadcast"}, true);
    ConfigFile::mConsoleLog       = Config->getValue<bool>({"Basic", "ConsoleLog"}, true);
    ConfigFile::mSendToast        = Config->getValue<bool>({"Basic", "SendToast"}, true);
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