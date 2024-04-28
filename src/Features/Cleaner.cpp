#include "Cleaner.h"

namespace Cleaner {

void loadCleaner() {
    auto& config = Cleaner::Entry::getInstance().getConfig();
    if (config.ScheduleClean.Enabled) {
        Cleaner::AutoCleanTask(config.ScheduleClean.CleanInterval);
    }
    if (config.AutoCleanCount.Enabled) {
        Cleaner::CleanTaskCount(config.AutoCleanCount.TriggerCount);
    }
    if (config.AutoCleanTPS.Enabled) {
        Cleaner::CleanTaskTPS(config.AutoCleanTPS.TriggerTPS);
    }
}

void unloadCleaner() { stopAllTasks(); }

void reloadCleaner() {
    Cleaner::Entry::getInstance().disable();
    Cleaner::Entry::getInstance().enable();
}

} // namespace Cleaner