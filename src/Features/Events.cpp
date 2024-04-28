#include "Cleaner.h"

namespace Cleaner {

void setShouldIgnore(GMLIB_Actor* ac) { ac->addTag("cleaner:ignore"); }

void ListenEvents() {
    auto& eventBus = ll::event::EventBus::getInstance();
    auto& config   = Cleaner::Entry::getInstance().getConfig();
    // ItemSpawnEvent
    eventBus.emplaceListener<GMLIB::Event::EntityEvent::ItemActorSpawnAfterEvent>(
        [&config](GMLIB::Event::EntityEvent::ItemActorSpawnAfterEvent& event) {
            auto& item = event.getItemActor();
            if (event.getSpawner().has_value()) {
                auto pl = (GMLIB_Player*)event.getSpawner().as_ptr();
                if (pl->isPlayer() && !pl->isAlive()) { // Death Drop
                    auto ac = (GMLIB_Actor*)&item;
                    setShouldIgnore(ac);
                    return;
                }
            }
            if (config.ItemDespawn.Enabled) {
                item.lifeTime() = config.ItemDespawn.DespawnTime;
                auto list       = config.ItemDespawn.WhiteList;
                auto type       = item.item().getTypeName();
                for (auto& key : list) {
                    if (isMatch(type, key)) {
                        return;
                    }
                }
                item.lifeTime() = config.ItemDespawn.DespawnTime;
            }
        }
    );
    // MobTakeItemEvent
    eventBus.emplaceListener<GMLIB::Event::EntityEvent::MobPickupItemAfterEvent>(
        [](GMLIB::Event::EntityEvent::MobPickupItemAfterEvent& event) {
            auto mob = (GMLIB_Actor*)&event.self();
            setShouldIgnore(mob);
        }
    );
}

} // namespace Cleaner