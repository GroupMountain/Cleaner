#include "Cleaner.h"

namespace Cleaner {

void setShouldIgnore(gmlib::world::Actor* ac) { ac->addTag("cleaner:ignore"); }

void ListenEvents() {
    auto& eventBus = ll::event::EventBus::getInstance();
    auto& config   = Cleaner::Entry::getInstance().getConfig();
    // ItemSpawnEvent
    eventBus.emplaceListener<gmlib::event::entity::ItemActorSpawnAfterEvent>(
        [&config](gmlib::event::entity::ItemActorSpawnAfterEvent& event) {
            auto& item = event.getItemActor();
            if (event.getSpawner().has_value()) {
                auto pl = (gmlib::world::Player*)event.getSpawner().as_ptr();
                if (pl->isPlayer() && !pl->isAlive()) { // Death Drop
                    auto ac = (gmlib::world::Actor*)&item;
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
    eventBus.emplaceListener<gmlib::event::entity::MobPickupItemAfterEvent>(
        [](gmlib::event::entity::MobPickupItemAfterEvent& event) {
            auto mob = (gmlib::world::Actor*)&event.self();
            setShouldIgnore(mob);
        }
    );
}

} // namespace Cleaner