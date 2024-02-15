#include "Config.h"
#include "Global.h"
#include <GMLIB/Event/Entity/ItemActorSpawnEvent.h>
#include <GMLIB/Event/Entity/MobPickupItemEvent.h>
#include <GMLIB/Server/PlayerAPI.h>
#include <regex>

using namespace ll::schedule;
using namespace ll::chrono_literals;

int item_despawn_time = 3000;

ServerTimeScheduler scheduler;
bool                auto_clean_triggerred = false;

bool ConsoleLog = true;
bool Announce   = true;

namespace Cleaner {

bool isMatch(std::string& A, std::string& B) {
    // 如果B是正则表达式
    if (B.find_first_of(".*+?()[]{}|^$") != std::string::npos) {
        try {
            std::regex regex(B);
            return std::regex_match(A, regex);
        } catch (...) {
            return false;
        }
    }
    return (A == B);
}

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
        if (Config->getValue<bool>({"CleanItem", "Enabled"}, false)) {
            auto itac = (ItemActor*)en;
            if (itac->age() <= Config->getValue<int>({"CleanItem", "ExistTicks"}, 0)) {
                return false;
            }
            auto itemType  = itac->item().getTypeName();
            auto whitelist = Config->getValue<std::vector<std::string>>({"CleanItem", "Whitelist"}, {});
            for (auto& key : whitelist) {
                if (isMatch(itemType, key)) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    // Mobs
    else if (en->isMob()) {
        if (Config->getValue<bool>({"CleanMobs", "Enabled"}, false)) {
            auto blacklist = Config->getValue<std::vector<std::string>>({"CleanMobs", "Blacklist"}, {});
            for (auto& key : blacklist) {
                if (isMatch(type, key)) {
                    return true;
                }
            }
            auto whitelist = Config->getValue<std::vector<std::string>>({"CleanMobs", "Whitelist"}, {});
            for (auto& key : whitelist) {
                if (isMatch(type, key)) {
                    return false;
                }
            }
            if (Config->getValue<bool>({"CleanMobs", "CleanMonstors"}, false)
                && en->hasCategory(ActorCategory::Monster)) {
                return true;
            }
            if (Config->getValue<bool>({"CleanMobs", "CleanPeacefulMobs"}, false)) {
                return true;
            }
        }
        return false;
    }
    // Others
    else {
        if (Config->getValue<bool>({"CleanInanimate", "Enabled"}, false)) {
            auto blacklist = Config->getValue<std::vector<std::string>>({"CleanInanimate", "Blacklist"}, {});
            for (auto& key : blacklist) {
                if (isMatch(type, key)) {
                    return true;
                }
            }
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

void CleanTask() {
    auto time          = Config->getValue<int>({"Basic", "Notice1"}, 20);
    auto announce_time = Config->getValue<int>({"Basic", "Notice2"}, 5);
    auto time_1        = std::chrono::seconds::duration(time);
    auto time_2        = std::chrono::seconds::duration(time - announce_time);
    if (ConsoleLog) {
        logger.info(tr("cleaner.output.count1", {S(time_1.count())}));
    }
    if (Announce) {
        TextPacket::createRawMessage(tr("cleaner.info.prefix") + tr("cleaner.output.count1", {S(time_1.count())}))
            .sendToClients();
    }
    scheduler.add<DelayTask>(time_2, [announce_time] {
        if (ConsoleLog) {
            logger.info(tr("cleaner.output.count2", {S(announce_time)}));
        }
        if (Announce) {
            TextPacket::createRawMessage(tr("cleaner.info.prefix") + tr("cleaner.output.count2", {S(announce_time)}))
                .sendToClients();
        }
    });
    scheduler.add<DelayTask>(time_1, [] {
        auto count = ExecuteClean();
        if (ConsoleLog) {
            logger.info(tr("cleaner.output.finish", {S(count)}));
        }
        if (Announce) {
            TextPacket::createRawMessage(tr("cleaner.info.prefix") + tr("cleaner.output.finish", {S(count)}))
                .sendToClients();
        }
        auto_clean_triggerred = false;
    });
}

void AutoCleanTask(int seconds) {
    auto time      = std::chrono::seconds::duration(seconds);
    mAutoCleanTask = scheduler.add<RepeatTask>(time, [] { CleanTask(); });
}

void CheckCleanTask(int max_entities, float min_tps) {
    mCheckCleanTask = scheduler.add<RepeatTask>(10s, [max_entities, min_tps] {
        auto count = CountEntities();
        if (auto_clean_triggerred == false) {
            if (count >= max_entities) {
                auto_clean_triggerred = true;
                logger.warn(tr("cleaner.output.triggerAutoCleanCount", {S(count)}));
                CleanTask();
            } else if (GMLIB_Level::getLevel()->getServerAverageTps() <= min_tps) {
                auto_clean_triggerred = true;
                logger.warn(
                    tr("cleaner.output.triggerAutoCleanCount", {S(GMLIB_Level::getLevel()->getServerAverageTps())})
                );
                CleanTask();
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

void reloadCleaner() {
    unloadCleaner();
    loadCleaner();
}

void loadCleaner() {
    ListenEvents();
    RegisterCommands();
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
    item_despawn_time = Config->getValue<int>({"ItemDespawn", "DespawnTime"}, 6000);
    Announce          = Config->getValue<bool>({"Basic", "SendBroadcast"}, true);
    ConsoleLog        = Config->getValue<bool>({"Basic", "ConsoleLog"}, true);
}

void unloadCleaner() {
    mAutoCleanTask->cancel();
    mCheckCleanTask->cancel();
    delete Config;
}

} // namespace Cleaner