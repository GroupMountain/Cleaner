#include "Global.h"

struct CleanerParam {
    enum class Despawn { despawn } despawn;
    enum class Action { tps, clean, reload, mspt } action;
    enum class DespawnTime { despawntime } despawntime;
    int                    ticks;
    CommandSelector<Actor> entity;
};

void RegCleanerCommand() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        Config->getValue<std::string>({"Basic", "Command"}, "cleaner"),
        tr("cleaner.command.cleaner"),
        CommandPermissionLevel::GameDirectors
    );
    cmd.overload<CleanerParam>()
        .required("despawn")
        .required("entity")
        .execute<[&](CommandOrigin const& origin, CommandOutput& output, CleanerParam const& param) {
            auto ens = param.entity.results(origin);
            if (ens.empty()) {
                return output.error(tr("cleaner.command.error.noTarget"));
            }
            for (auto en : ens) {
                en->despawn();
            }
            return output.success(tr("cleaner.command.despawnSuccess", {S(ens.size())}));
        }>();
    cmd.overload<CleanerParam>()
        .required("action")
        .execute<[&](CommandOrigin const& origin, CommandOutput& output, CleanerParam const& param) {
            switch (param.action) {
            case CleanerParam::Action::clean: {
                Cleaner::CleanTask();
                return output.success(tr("cleaner.output.opClean"));
            }
            case CleanerParam::Action::tps: {
                return output.success(
                    tr("cleaner.command.tps.output",
                       {S(GMLIB_Level::getLevel()->getServerCurrentTps()),
                        S(GMLIB_Level::getLevel()->getServerAverageTps())})
                );
            }
            case CleanerParam::Action::mspt: {
                return output.success(tr("cleaner.command.mspt.output", {S(GMLIB_Level::getLevel()->getServerMspt())}));
            }
            case CleanerParam::Action::reload: {
                Cleaner::reloadCleaner();
                return output.success(tr("cleaner.output.reload"));
            }
            }
        }>();
    cmd.overload<CleanerParam>()
        .required("despawntime")
        .required("ticks")
        .execute<[&](CommandOrigin const& origin, CommandOutput& output, CleanerParam const& param) {
            item_despawn_time = param.ticks;
            Config->setValue({"ItemDespawn", "DespawnTime"}, item_despawn_time);
            return output.success(tr("cleaner.command.despawntime", {S(item_despawn_time)}));
        }>();
};

void RegisterCommands() {
    RegCleanerCommand();
    // RegVoteCommand(registry);
}

/*
void RegVoteCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(registry, "voteclean", "Clean entities", CommandPermissionLevel::Any);
    command->addOverload();
    command->setCallback([](DynamicCommand const&                                    cmd,
                            CommandOrigin const&                                     origin,
                            CommandOutput&                                           output,
                            std::unordered_map<std::string, DynamicCommand::Result>& result) {
        // vote clean command
    });
    DynamicCommand::setup(registry, std::move(command));
}
*/