#include "Global.h"

using namespace ll::schedule;
using namespace ll::chrono_literals;

GameTimeScheduler scheduler;

namespace Cleaner {

bool isDeathDrop(ItemActor* itac) {
    // Todo
    return false;
}

bool shouldIgnoreMob(Mob* mob) {
    // Todo
    return false;
}

bool ShouldClean(Actor* en) {
    // Players
    if (en->isType(ActorType::Player)) {
        return false;
    }
    // Tag
    if (en->hasTag("testtag")) {
        // Todo
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
                return true;
                return false;
            }
            return true;
        }
        return false;
    }
    // Mobs
    else if (en->hasCategory(ActorCategory::Mob)) {
        if (true) { // Todo Config Toggle
            auto mob = (Mob*)en;
            if (shouldIgnoreMob(mob)) {
                return false;
            }
            if (true) { // Todo WhiteList
                return true;
                return false;
            }
        }
        return false;
    }
    // Others
    else {
        if (true) {     // Todo Config Toggle
            if (true) { // BlackList
                return true;
            }
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

using namespace ll::schedule;

using namespace ll::chrono_literals;

void CleanTask() {
    // Todo Schedule
    logger.info("clean will in {}", 20s);
    scheduler.add<DelayTask>(20s, [] {
        auto count = ExecuteClean();
        logger.info("Successfully cleaned {} entities", count);
    });
}

void CountEntitiesTask() {
    //
}

} // namespace Cleaner