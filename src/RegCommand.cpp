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
        Cleaner::CleanSchedule(20, 5);
    });
    DynamicCommand::setup(registry, std::move(command));
}

void RegReloadCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(registry, "cleane", "Clean entities", CommandPermissionLevel::Any);
    command->addOverload();
    command->setCallback([](DynamicCommand const&                                    cmd,
                            CommandOrigin const&                                     origin,
                            CommandOutput&                                           output,
                            std::unordered_map<std::string, DynamicCommand::Result>& result) {
        // reload command
    });
    DynamicCommand::setup(registry, std::move(command));
}

void RegDespawnCommand(CommandRegistry& registry) {
    auto command = DynamicCommand::createCommand(registry, "cleanee", "Clean entities", CommandPermissionLevel::Any);
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
        // despawn command
    });
    DynamicCommand::setup(registry, std::move(command));
}

void RegisterCommand(CommandRegistry &registry) {
    RegCleanCommand(registry);
}