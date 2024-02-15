#include "Global.h"

ll::Logger logger("Cleaner");

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    // Code for loading the plugin goes here.
    initPlugin();
}

bool Plugin::enable() {
    Cleaner::loadCleaner();
    logger.info("Cleaner Loaded!");
    logger.info("Author: GroupMountain");
    logger.info("Repository: https://github.com/GroupMountain/Cleaner");
    return true;
}

bool Plugin::disable() {
    // Code for disabling the plugin goes here.
    Cleaner::unloadCleaner();
    logger.info("Cleaner Disabled!");
    return true;
}

} // namespace plugin