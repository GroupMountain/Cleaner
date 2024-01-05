#include "Global.h"

ll::Logger logger("Cleaner");

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    // Code for loading the plugin goes here.
}

bool Plugin::enable() {
    //TPS::CaculateTPS();
    RegisterCommands();
    item_despawn_time = 300;
    Cleaner::AutoCleanTask(60);
    Cleaner::CheckCleanTask(20, 15);
    logger.info("Cleaner Loaded!");
    logger.info("Author: Tsubasa6848");
    logger.info("Repository: https://github.com/GroupMountain/Cleaner");
    return true;
}

bool Plugin::disable() {
    logger.info("Disabling Cleaner...");
    // Code for disabling the plugin goes here.
    //auto_clean_task->cancel();
    //check_clean_task->cancel();
    UnregisterCommands();
    logger.info("Cleaner Disabled!");
    return true;
}

} // namespace plugin