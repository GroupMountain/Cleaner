#include "Global.h"

using ParamType = DynamicCommand::ParameterType;

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

void RegCleanerCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(
        registry,
        Config->getValue<std::string>({"Basic", "Command"}, "cleaner"),
        Language->translate("cleaner.command.cleaner"),
        CommandPermissionLevel::GameDirectors
    );
    command->setEnum("Despawn", {"despawn"});
    command->setEnum("Action", {"tps", "clean", "reload", "mspt"});
    command->setEnum("Despawntime", {"despawntime"});
    command->mandatory("action", ParamType::Enum, "Action", CommandParameterOption::EnumAutocompleteExpansion);
    command
        ->mandatory("despawntime", ParamType::Enum, "Despawntime", CommandParameterOption::EnumAutocompleteExpansion);
    command->mandatory("despawn", ParamType::Enum, "Despawn", CommandParameterOption::EnumAutocompleteExpansion);
    command->mandatory("ticks", ParamType::Int);
    command->mandatory("target", ParamType::Actor);
    command->addOverload({"action"});
    command->addOverload({"despawntime", "ticks"});
    command->addOverload({"despawn", "target"});
    command->setCallback([](DynamicCommand const&                                    cmd,
                            CommandOrigin const&                                     origin,
                            CommandOutput&                                           output,
                            std::unordered_map<std::string, DynamicCommand::Result>& result) {
        if (result["despawn"].isSet && result["target"].isSet) {
            auto ens = result["target"].get<std::vector<Actor*>>();
            if (ens.size() == 0) {
                return output.error(Language->translate("cleaner.command.error.noTarget"));
            }
            for (auto en : ens) {
                en->despawn();
            }
            return output.success(Language->translate("cleaner.command.despawnSuccess", {S(ens.size())}));
        } else if (result["action"].isSet) {
            auto act = result["action"].get<std::string>();
            if (act == "tps") {
                return output.success(Language->translate(
                    "cleaner.command.tps.output",
                    {S(GMLIB_Level::getLevel()->getServerCurrentTps()),
                     S(GMLIB_Level::getLevel()->getServerAverageTps())}
                ));
            } else if (act == "mspt") {
                return output.success(
                    Language->translate("cleaner.command.mspt.output", {S(GMLIB_Level::getLevel()->getServerMspt())})
                );
            } else if (act == "clean") {
                Cleaner::CleanTask(
                    Config->getValue<int>({"Basic", "Notice1"}, 20),
                    Config->getValue<int>({"Basic", "Notice2"}, 15)
                );
                return output.success(Language->translate("cleaner.output.opClean"));
            } else if (act == "reload") {
                Cleaner::reloadCleaner();
                return output.success(Language->translate("cleaner.output.reload"));
            }
        } else if (result["despawntime"].isSet && result["ticks"].isSet) {
            item_despawn_time = result["ticks"].get<int>();
            return output.success(Language->translate("cleaner.command.despawntime", {S(item_despawn_time)}));
        }
    });
    DynamicCommand::setup(registry, std::move(command));
}

void RegisterCommands() {
    auto registry = ll::service::bedrock::getCommandRegistry();
    // RegCleanerCommand(registry);
    // RegVoteCommand(registry);
}

void UnregisterCommands() {
    // auto registry = ll::service::bedrock::getCommandRegistry();
    // registry->unregisterCommand(Config->getValue<std::string>({"Basic", "Command"}, "cleaner"));
    //  registry->unregisterCommand("voteclean");
}