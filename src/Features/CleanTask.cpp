#include "Cleaner.h"

namespace ConfigFile {

bool                     mConsoleLog = true;
bool                     mAnnounce   = true;
bool                     mSendToast  = true;
std::vector<std::string> mIgnoreTags = {};

} // namespace ConfigFile


namespace Cleaner {

static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mAutoCleanTask  = nullptr;
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mCleanTaskCount = nullptr;
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mCleanTaskTPS   = nullptr;

ServerTimeScheduler scheduler;

bool auto_clean_triggerred = false;

void CleanTask() {
    auto time          = Config->getValue<int>({"Basic", "Notice1"}, 20);
    auto announce_time = Config->getValue<int>({"Basic", "Notice2"}, 5);
    auto time_1        = std::chrono::seconds::duration(time);
    auto time_2        = std::chrono::seconds::duration(time - announce_time);
    if (ConfigFile::mConsoleLog) {
        logger.info(tr("cleaner.output.count1", {S(time_1.count())}));
    }
    if (ConfigFile::mAnnounce) {
        Helper::broadcastMessage(tr("cleaner.output.count1", {S(time_1.count())}));
    }
    if (ConfigFile::mSendToast) {
        Helper::broadcastToast(tr("cleaner.output.count2", {S(announce_time)}));
    }
    scheduler.add<DelayTask>(time_2, [announce_time] {
        if (ConfigFile::mConsoleLog) {
            logger.info(tr("cleaner.output.count2", {S(announce_time)}));
        }
        if (ConfigFile::mAnnounce) {
            Helper::broadcastMessage(tr("cleaner.output.count2", {S(announce_time)}));
        }
        if (ConfigFile::mSendToast) {
            Helper::broadcastToast(tr("cleaner.output.count2", {S(announce_time)}));
        }
    });
    scheduler.add<DelayTask>(time_1, [] {
        auto count = ExecuteClean();
        if (ConfigFile::mConsoleLog) {
            logger.info(tr("cleaner.output.finish", {S(count)}));
        }
        if (ConfigFile::mAnnounce) {
            Helper::broadcastMessage(tr("cleaner.output.finish", {S(count)}));
        }
        if (ConfigFile::mSendToast) {
            Helper::broadcastToast(tr("cleaner.output.finish", {S(count)}));
        }
        auto_clean_triggerred = false;
    });
}

void AutoCleanTask(int seconds) {
    auto time      = std::chrono::seconds::duration(seconds);
    mAutoCleanTask = scheduler.add<RepeatTask>(time, [] { CleanTask(); });
}

void CleanTaskCount(int max_entities) {
    mCleanTaskCount = scheduler.add<RepeatTask>(10s, [max_entities] {
        auto count = CountEntities();
        if (auto_clean_triggerred == false) {
            if (count >= max_entities) {
                auto_clean_triggerred = true;
                if (ConfigFile::mConsoleLog) {
                    logger.warn(tr("cleaner.output.triggerAutoCleanCount", {S(count)}));
                }
                if (ConfigFile::mAnnounce) {
                    Helper::broadcastMessage(tr("cleaner.output.triggerAutoCleanCount", {S(count)}));
                }
                if (ConfigFile::mSendToast) {
                    Helper::broadcastToast(tr("cleaner.output.triggerAutoCleanCount", {S(count)}));
                }
                CleanTask();
            }
        }
    });
}

void CleanTaskTPS(float min_tps) {
    mCleanTaskTPS = scheduler.add<RepeatTask>(10s, [min_tps] {
        if (auto_clean_triggerred == false) {
            if (GMLIB_Level::getLevel()->getServerAverageTps() <= min_tps) {
                auto_clean_triggerred = true;
                auto mspt             = S(GMLIB_Level::getLevel()->getServerAverageTps());
                if (ConfigFile::mConsoleLog) {
                    logger.warn(tr("cleaner.output.triggerAutoCleanTps", {mspt}));
                }
                if (ConfigFile::mAnnounce) {
                    Helper::broadcastMessage(tr("cleaner.output.triggerAutoCleanTps", {mspt}));
                }
                if (ConfigFile::mSendToast) {
                    Helper::broadcastToast(tr("cleaner.output.triggerAutoCleanTps", {mspt}));
                }
                CleanTask();
            }
        }
    });
}

void stopAllTasks() {
    if (!mCleanTaskCount) {
        mCleanTaskTPS->cancel();
        scheduler.remove(mCleanTaskTPS);
    }
    if (mCleanTaskCount) {
        mCleanTaskCount->cancel();
        scheduler.remove(mCleanTaskCount);
    }
    if (mAutoCleanTask) {
        mAutoCleanTask->cancel();
        scheduler.remove(mAutoCleanTask);
    }
}
} // namespace Cleaner
