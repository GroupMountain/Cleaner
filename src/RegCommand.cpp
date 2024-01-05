#include "Global.h"

using ParamType = DynamicCommand::ParameterType;

void RegVoteCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(registry, "voteclean", "Clean entities", CommandPermissionLevel::Any);
    command->addOverload();
    command->setCallback([](DynamicCommand const&                                    cmd,
                            CommandOrigin const&                                     origin,
                            CommandOutput&                                           output,
                            std::unordered_map<std::string, DynamicCommand::Result>& result) {
        // vote clean
    });
    DynamicCommand::setup(registry, std::move(command));
}

void RegCleanCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(registry, "clean", "Clean entities", CommandPermissionLevel::Any);
    command->addOverload();
    command->setCallback([](DynamicCommand const&                                    cmd,
                            CommandOrigin const&                                     origin,
                            CommandOutput&                                           output,
                            std::unordered_map<std::string, DynamicCommand::Result>& result) {
        Cleaner::CleanTask(15, 7);
    });
    DynamicCommand::setup(registry, std::move(command));
}

void RegConfigCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(registry, "cleaner", "Cleaner plugin settings", CommandPermissionLevel::Any);
    command->addOverload();
    command->setCallback([](DynamicCommand const&                                    cmd,
                            CommandOrigin const&                                     origin,
                            CommandOutput&                                           output,
                            std::unordered_map<std::string, DynamicCommand::Result>& result) {
        // reload & config
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
        auto ens = result["target"].get<std::vector<Actor*>>();
        for (auto en : ens) {
            en->despawn();
        }
    });
    DynamicCommand::setup(registry, std::move(command));
}

void RegisterCommands() {
    auto registry = ll::service::bedrock::getCommandRegistry();
    RegCleanCommand(registry);
    RegDespawnCommand(registry);
}

void UnregisterCommands() {
    auto registry = ll::service::bedrock::getCommandRegistry();
    registry->unregisterCommand("clean");
    registry->unregisterCommand("despawn");
}