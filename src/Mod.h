#pragma once

#include "Config.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/schedule/Scheduler.h>

namespace Cleaner {

using namespace ll::schedule;

class Entry {

public:
    static Entry& getInstance();

    Entry(ll::mod::NativeMod& self) : mSelf(self) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    /// @return True if the mod is loaded successfully.
    bool load();

    /// @return True if the mod is enabled successfully.
    bool enable();

    /// @return True if the mod is disabled successfully.
    bool disable();

    // TODO: Implement this method if you need to unload the mod.
    // /// @return True if the mod is unloaded successfully.
    // bool unload();

    Config& getConfig();

    void saveConfig();

    ServerTimeScheduler& getScheduler();

private:
    ll::mod::NativeMod&                mSelf;
    std::optional<Config>              mConfig;
    std::optional<ServerTimeScheduler> mScheduler;
};

} // namespace Cleaner
