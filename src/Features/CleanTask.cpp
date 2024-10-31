#include "Cleaner.h"

namespace Cleaner {

static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mAutoCleanTask  = nullptr;
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mCleanTaskCount = nullptr;
static std::shared_ptr<ll::schedule::task::Task<ll::chrono::ServerClock>> mCleanTaskTPS   = nullptr;

bool auto_clean_triggerred = false;

void CleanTask() {
    auto& config        = Cleaner::Entry::getInstance().getConfig();
    auto  time          = config.Basic.Notice1;
    auto  announce_time = config.Basic.Notice2;
    auto  time_1        = std::chrono::seconds::duration(time);
    auto  time_2        = std::chrono::seconds::duration(time - announce_time);
    if (config.Basic.ConsoleLog) {
        logger.info(tr("cleaner.output.count1", {S(time_1.count())}));
    }
    if (config.Basic.SendBroadcast) {
        Helper::broadcastMessage(tr("cleaner.output.count1", {S(time_1.count())}));
    }
    if (config.Basic.SendToast) {
        Helper::broadcastToast(tr("cleaner.output.count2", {S(announce_time)}));
    }
    Cleaner::Entry::getInstance().getScheduler().add<DelayTask>(time_2, [announce_time, &config] {
        if (config.Basic.ConsoleLog) {
            logger.info(tr("cleaner.output.count2", {S(announce_time)}));
        }
        if (config.Basic.SendBroadcast) {
            Helper::broadcastMessage(tr("cleaner.output.count2", {S(announce_time)}));
        }
        if (config.Basic.SendToast) {
            Helper::broadcastToast(tr("cleaner.output.count2", {S(announce_time)}));
        }
    });
    Cleaner::Entry::getInstance().getScheduler().add<DelayTask>(time_1, [&config] {
        auto count = ExecuteClean();
        if (config.Basic.ConsoleLog) {
            logger.info(tr("cleaner.output.finish", {S(count)}));
        }
        if (config.Basic.SendBroadcast) {
            Helper::broadcastMessage(tr("cleaner.output.finish", {S(count)}));
        }
        if (config.Basic.SendToast) {
            Helper::broadcastToast(tr("cleaner.output.finish", {S(count)}));
        }
        auto_clean_triggerred = false;
    });
}

void AutoCleanTask(int seconds) {
    auto time      = std::chrono::seconds::duration(seconds);
    mAutoCleanTask = Cleaner::Entry::getInstance().getScheduler().add<RepeatTask>(time, [] { CleanTask(); });
}

void CleanTaskCount(int max_entities) {
    auto& config    = Cleaner::Entry::getInstance().getConfig();
    mCleanTaskCount = Cleaner::Entry::getInstance().getScheduler().add<RepeatTask>(10s, [max_entities, &config] {
        auto count = CountEntities();
        if (auto_clean_triggerred == false) {
            if (count >= max_entities) {
                auto_clean_triggerred = true;
                if (config.Basic.ConsoleLog) {
                    logger.warn(tr("cleaner.output.triggerAutoCleanCount", {S(count)}));
                }
                if (config.Basic.SendBroadcast) {
                    Helper::broadcastMessage(tr("cleaner.output.triggerAutoCleanCount", {S(count)}));
                }
                if (config.Basic.SendToast) {
                    Helper::broadcastToast(tr("cleaner.output.triggerAutoCleanCount", {S(count)}));
                }
                CleanTask();
            }
        }
    });
}

void CleanTaskTPS(float min_tps) {
    auto& config  = Cleaner::Entry::getInstance().getConfig();
    mCleanTaskTPS = Cleaner::Entry::getInstance().getScheduler().add<RepeatTask>(10s, [min_tps, &config] {
        if (auto_clean_triggerred == false) {
            if (gmlib::world::Level::getLevel()->getServerAverageTps() <= min_tps) {
                auto_clean_triggerred = true;
                auto mspt             = S(gmlib::world::Level::getLevel()->getServerAverageTps());
                if (config.Basic.ConsoleLog) {
                    logger.warn(tr("cleaner.output.triggerAutoCleanTps", {mspt}));
                }
                if (config.Basic.SendBroadcast) {
                    Helper::broadcastMessage(tr("cleaner.output.triggerAutoCleanTps", {mspt}));
                }
                if (config.Basic.SendToast) {
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
        Cleaner::Entry::getInstance().getScheduler().remove(mCleanTaskTPS);
    }
    if (mCleanTaskCount) {
        mCleanTaskCount->cancel();
        Cleaner::Entry::getInstance().getScheduler().remove(mCleanTaskCount);
    }
    if (mAutoCleanTask) {
        mAutoCleanTask->cancel();
        Cleaner::Entry::getInstance().getScheduler().remove(mAutoCleanTask);
    }
}
} // namespace Cleaner
