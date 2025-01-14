#include "Features/Cleaner.h"

struct CleanerParam {
    enum class Despawn { despawn } despawn;
    enum class Action { tps, clean, reload, mspt } action;
    enum class DespawnTime { despawntime } despawntime;
    int                    ticks;
    CommandSelector<Actor> entity;
};

void RegCleanerCommand() {
    auto& config = Cleaner::Entry::getInstance().getConfig();
    auto& cmd    = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        config.Basic.Command,
        tr("cleaner.command.cleaner"),
        CommandPermissionLevel::GameDirectors
    );
    cmd.overload<CleanerParam>()
        .required("despawn")
        .required("entity")
        .execute([&](CommandOrigin const& origin, CommandOutput& output, CleanerParam const& param) {
            auto ens = param.entity.results(origin);
            if (ens.empty()) {
                return output.error(tr("cleaner.command.error.noTarget"));
            }
            for (auto en : ens) {
                en->despawn();
            }
            return output.success(tr("cleaner.command.despawnSuccess", {S(ens.size())}));
        });
    cmd.overload<CleanerParam>()
        .required("action")
        .execute([&](CommandOrigin const& origin, CommandOutput& output, CleanerParam const& param) {
            switch (param.action) {
            case CleanerParam::Action::clean: {
                Cleaner::CleanTask();
                if (Cleaner::Entry::getInstance().getConfig().Basic.ConsoleLog) {
                    ll::io::LoggerRegistry::getInstance().getOrCreate("Cleaner")->info(tr("cleaner.output.opClean"));
                }
                if (Cleaner::Entry::getInstance().getConfig().Basic.SendBroadcast) {
                    Helper::broadcastMessage(tr("cleaner.output.opClean"));
                }
                if (Cleaner::Entry::getInstance().getConfig().Basic.SendToast) {
                    Helper::broadcastToast(tr("cleaner.output.opClean"));
                }
                return output.success(tr("cleaner.command.clean.output"));
            }
            case CleanerParam::Action::tps: {
                return output.success(tr(
                    "cleaner.command.tps.output",
                    {S(GMLIB_Level::getLevel()->getServerCurrentTps()),
                     S(GMLIB_Level::getLevel()->getServerAverageTps())}
                ));
            }
            case CleanerParam::Action::mspt: {
                return output.success(
                    tr("cleaner.command.mspt.output", {S(GMLIB_Level::getLevel()->getServerMspt())})
                );
            }
            case CleanerParam::Action::reload: {
                Cleaner::reloadCleaner();
                return output.success(tr("cleaner.output.reload"));
            }
            }
        });
    cmd.overload<CleanerParam>()
        .required("despawntime")
        .required("ticks")
        .execute([&](CommandOrigin const& origin, CommandOutput& output, CleanerParam const& param) {
            Cleaner::Entry::getInstance().getConfig().ItemDespawn.DespawnTime = param.ticks;
            Cleaner::Entry::getInstance().saveConfig();
            return output.success(tr("cleaner.command.despawntime", {S(param.ticks)}));
        });
};

void RegVoteCommand() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        Cleaner::Entry::getInstance().getConfig().VoteClean.VoteCleanCommand,
        tr("cleaner.command.voteclean"),
        CommandPermissionLevel::Any
    );
    cmd.overload().execute([&](CommandOrigin const& origin, CommandOutput& output) {
        if (origin.getOriginType() == CommandOriginType::Player) {
            auto pl = (Player*)origin.getEntity();
            return VoteClean::voteCommandExecute(pl);
        }
        return output.error(tr("cleaner.command.error.playerOnly"));
    });
}

void RegisterCommands() {
    RegCleanerCommand();
    if (Cleaner::Entry::getInstance().getConfig().VoteClean.Enabled) {
        RegVoteCommand();
    }
}