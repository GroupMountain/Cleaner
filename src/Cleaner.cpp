#include "Global.h"
#include <GMLIB/Event/Entity/ItemActorSpawnEvent.h>
#include <GMLIB/Event/Entity/MobPickupItemEvent.h>
#include <GMLIB/Server/PlayerAPI.h>

using namespace ll::schedule;
using namespace ll::chrono_literals;
int item_despawn_time = 3000;

ServerTimeAsyncScheduler scheduler;
bool                     auto_clean_triggerred = false;

namespace Cleaner {

bool shouldIgnore(GMLIB_Actor* ac) {
    if (ac->isMob() || ac->isItemActor()) {
        if (ac->isTame() || ac->isTrusting() || ac->getNameTag() != "") {
            return true;
        }
        auto nbt = ac->getNbt();
        if (nbt->getByte("ShowBottom") == 1) { // End Crystal Used Only, if has this tag 1b, it is modified by cleaner.
            return true;
        }
    }
    return false;
}

bool ShouldClean(Actor* actor) {
    // Players
    auto en = (GMLIB_Actor*)actor;
    if (en->isPlayer() || shouldIgnore(en)) {
        return false;
    }
    auto type = en->getTypeName();
    // Items
    if (en->isItemActor()) {
        return true;
        if (true) { // Todo Config Toggle
            auto itac     = (ItemActor*)en;
            auto itemType = itac->item().getTypeName();
            if (true) { // Todo Config WhiteList
                return false;
            }
            return true;
        }
        return false;
    }
    // Mobs
    else if (en->isMob()) {
        if (true) { // Todo Config Toggle
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
    auto all_entities = GMLIB_Level::getLevel()->getAllEntities();
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
    auto all_entities = GMLIB_Level::getLevel()->getAllEntities();
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
    auto time      = std::chrono::seconds::duration(seconds);
    mAutoCleanTask = scheduler.add<RepeatTask>(time, [] { CleanTask(20, 5); });
}

void CheckCleanTask(int max_entities, float min_tps) {
    mCheckCleanTask = scheduler.add<RepeatTask>(10s, [max_entities, min_tps] {
        auto count = CountEntities();
        if (auto_clean_triggerred == false) {
            if (count >= max_entities) {
                auto_clean_triggerred = true;
                logger.warn("Too many entities! Auto started clean task!");
                CleanTask(20, 5);
            } else if (GMLIB_Level::getLevel()->getServerAverageTps() <= min_tps) {
                auto_clean_triggerred = true;
                logger.warn("TPS too low! Auto started clean task!");
                CleanTask(20, 5);
            }
        }
    });
}

void setShouldIgnore(GMLIB_Actor* ac) {
    auto nbt = ac->getNbt();
    nbt->put("ShowBottom", ByteTag(1));
    ac->setNbt(*nbt);
}

void ListenEvents() {
    auto eventBus = &ll::event::EventBus::getInstance();
    // ItemSpawnEvent
    eventBus->emplaceListener<GMLIB::Event::EntityEvent::ItemActorSpawnAfterEvent>(
        [](GMLIB::Event::EntityEvent::ItemActorSpawnAfterEvent& event) {
            auto& item = event.getItemActor();
            auto  pl   = (GMLIB_Player*)event.getSpawner();
            if (pl) {
                if (pl->isPlayer() && !pl->isAlive()) { // Death Drop
                    auto ac = (GMLIB_Actor*)&item;
                    setShouldIgnore(ac);
                    return;
                }
            }
            item.lifeTime() = item_despawn_time;
        }
    );
    // MobTakeItemEvent
    eventBus->emplaceListener<GMLIB::Event::EntityEvent::MobPickupItemAfterEvent>(
        [](GMLIB::Event::EntityEvent::MobPickupItemAfterEvent& event) {
            // auto item = event.getItemActor().item().getTypeName();
            auto mob = (GMLIB_Actor*)&event.self();
            setShouldIgnore(mob);
        }
    );
}

void ReloadCleaner() {
    mAutoCleanTask->cancel();
    mCheckCleanTask->cancel();
    // UnregisterCommands();
    // RegisterCommands();
    Cleaner::AutoCleanTask(600);
    Cleaner::CheckCleanTask(15, 8);
}

} // namespace Cleaner