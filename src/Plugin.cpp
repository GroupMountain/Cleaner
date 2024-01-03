#include "Global.h"

ll::Logger logger("Cleaner");

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    // Code for loading the plugin goes here.
    auto& eventBus             = ll::event::EventBus::getInstance();
    mSetupCommandEventListener = eventBus.emplaceListener<ll::event::command::SetupCommandEvent>([](ll::event::command::SetupCommandEvent& event) {
        RegisterCommand(event.registry());
    });  
}

bool Plugin::enable() {
    auto& logger = mSelf.getLogger();
    logger.info("Cleaner Loaded!");
    logger.info("Author: Tsubasa6848");
    logger.info("Repository: https://github.com/GroupMountain/Cleaner");
    return true;
}

bool Plugin::disable() {
    auto& logger = mSelf.getLogger();
    logger.info("Disabling Cleaner...");
    // Code for disabling the plugin goes here.
    
    logger.info("Cleaner Disabled!");
    return true;
}

} // namespace plugin