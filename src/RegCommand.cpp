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
        "cleaner",
        "Cleaner plugin settings",
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
                return output.error("No matched targets found!");
            }
            for (auto en : ens) {
                en->despawn();
            }
            return output.success("Successfully despawned {} entities", ens.size());
        } else if (result["action"].isSet) {
            auto act = result["action"].get<std::string>();
            if (act == "tps") {
                return output.success("current tps {}, average tps {}", TPS::getCurrentTps(), TPS::getAverageTps());
            } else if (act == "mspt") {
                return output.success("mspt {}", TPS::getMspt());
            } else if (act == "clean") {
                output.success("Clean task started!");
                Cleaner::CleanTask(20, 5);
                return;
            } else if (act == "reload") {
                output.success("Reloading Cleaner ...");
                return output.success("Cleaner Reloaded!");
            }
        } else if (result["despawntime"].isSet && result["ticks"].isSet) {
            item_despawn_time = result["ticks"].get<int>();
            return output.success("Successfully set deapawn time to {}", item_despawn_time);
        }
    });
    DynamicCommand::setup(registry, std::move(command));
}

void RegDespawnCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(registry, "despawn", "despawn entities", CommandPermissionLevel::Any);
    command->mandatory("target", ParamType::Actor);
    command->addOverload({"target"});
    command->addOverload();
    command->setCallback([](DynamicCommand const&                                    cmd,
                            CommandOrigin const&                                     origin,
                            CommandOutput&                                           output,
                            std::unordered_map<std::string, DynamicCommand::Result>& result) {

    });
    DynamicCommand::setup(registry, std::move(command));
}

void RegisterCommands() {
    auto registry = ll::service::bedrock::getCommandRegistry();
    RegCleanerCommand(registry);
    RegVoteCommand(registry);
}

void UnregisterCommands() {
    auto registry = ll::service::bedrock::getCommandRegistry();
    registry->unregisterCommand("cleaner");
    registry->unregisterCommand("voteclean");
}