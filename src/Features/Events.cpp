#include "Cleaner.h"

namespace Cleaner {
void setShouldIgnore(gmlib::GMActor* ac) { ac->addTag("cleaner:ignore"); }

void ListenEvents() {
    auto& eventBus = ll::event::EventBus::getInstance();
    auto& config   = Cleaner::Entry::getInstance().getConfig();
    // ItemSpawnEvent
    eventBus.emplaceListener<ila::mc::SpawnItemActorAfterEvent>([&config](ila::mc::SpawnItemActorAfterEvent& event) {
        auto& item = event.itemActor();
        if (event.spawner()) {
            auto pl = (gmlib::GMPlayer*)event.spawner();
            if (pl->isPlayer() && !pl->isAlive()) { // Death Drop
                auto ac = (gmlib::GMActor*)&item;
                setShouldIgnore(ac);
                return;
            }
        }
        if (config.ItemDespawn.Enabled) {
            // item.lifeTime() = config.ItemDespawn.DespawnTime;
            auto list        = config.ItemDespawn.WhiteList;
            auto type        = item.item().getTypeName();
            for (auto& key : list) {
                if (isMatch(type, key)) {
                    return;
                }
            }
            item.lifeTime() = config.ItemDespawn.DespawnTime;
        }
    });
    // MobTakeItemEvent
    eventBus.emplaceListener<ila::mc::ActorPickupItemAfterEvent>([](ila::mc::ActorPickupItemAfterEvent& event) {
        auto mob = (gmlib::GMActor*)&event.self();
        setShouldIgnore(mob);
    });
}

} // namespace Cleaner