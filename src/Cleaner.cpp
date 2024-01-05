#include "Global.h"

using namespace ll::schedule;
using namespace ll::chrono_literals;

GameTimeAsyncScheduler scheduler;
bool                   auto_clean_triggerred = false;

namespace Cleaner {

bool isDeathDrop(ItemActor* itac) {
    //
    return false;
}

bool shouldIgnoreMob(Actor* ac) {
    if (ac->isTame() || ac->isTrusting() || ac->getNameTag() != "") {
        return true;
    }

    return false;
}

bool ShouldClean(Actor* en) {
    // Players
    if (en->isType(ActorType::Player)) {
        return false;
    }
    auto type = en->getTypeName();
    // Items
    if (en->hasCategory(ActorCategory::Item)) {
        if (true) { // Todo Config Toggle
            auto itac = (ItemActor*)en;
            if (isDeathDrop(itac)) {
                return false;
            }
            auto itemType = itac->item().getTypeName();
            if (true) { // Todo Config WhiteList
                return false;
            }
            return true;
        }
        return false;
    }
    // Mobs
    else if (en->hasCategory(ActorCategory::Mob)) {
        if (true) { // Todo Config Toggle
            if (shouldIgnoreMob(en)) {
                return false;
            }
            // Test
            return true;
            if (true) { // Todo WhiteList
                return false;
            }
            return true;
        }
        return false;
    }
    // Others
    else {
        if (true) {     // Todo Config Toggle
            if (true) { // BlackList
                return true;
            }
            return false;
        }
        return false;
    }
}

int ExecuteClean() {
    int  clean_count  = 0;
    auto all_entities = ll::service::bedrock::getLevel()->getRuntimeActorList();
    for (auto entity : all_entities) {
        if (ShouldClean(entity)) {
            entity->despawn();
            clean_count++;
        }
    }
    return clean_count;
}

int CountEntities() {
    int  clean_count  = 0;
    auto all_entities = ll::service::bedrock::getLevel()->getRuntimeActorList();
    for (auto entity : all_entities) {
        if (ShouldClean(entity)) {
            clean_count++;
        }
    }
    return clean_count;
}

void CleanTask(int time, int announce_time) {
    auto time_1 = std::chrono::seconds::duration(time);
    auto time_2 = std::chrono::seconds::duration(time - announce_time);
    logger.info("clean will in {}", time_1);
    scheduler.add<DelayTask>(time_2, [announce_time] { logger.info("clean will in {}", announce_time); });
    scheduler.add<DelayTask>(time_1, [] {
        auto count = ExecuteClean();
        logger.info("Successfully cleaned {} entities", count);
        auto_clean_triggerred = false;
    });
}

void AutoCleanTask(int seconds) {
    auto time       = std::chrono::seconds::duration(seconds);
    auto_clean_task = scheduler.add<RepeatTask>(time, [] { CleanTask(20, 5); });
}

void CheckCleanTask(int max_entities, float min_tps) {
    check_clean_task = scheduler.add<RepeatTask>(10s, [max_entities, min_tps] {
        auto count = CountEntities();
        if (auto_clean_triggerred == false) {
            if (count >= max_entities) {
                auto_clean_triggerred = true;
                logger.warn("Too many entities! Auto started clean task!");
                CleanTask(20, 5);
            } else if (TPS::getAverageTps() <= min_tps) {
                auto_clean_triggerred = true;
                logger.warn("TPS too low! Auto started clean task!");
                CleanTask(20, 5);
            }
        }
    });
}


} // namespace Cleaner