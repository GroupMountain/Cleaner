#include <llapi/RegCommandAPI.h>
#include <llapi/DynamicCommandAPI.h>
#include "Global.h" 
#include "setting.h"
#include <llapi/mc/Level.hpp>

using namespace RegisterCommandHelper;
using namespace LanguageSettings;

extern void VoteForm(Player* player);
extern void _CleanerMain();

string endstr = "    §b§l---- Cleaner";

int ac_count = 0;
bool checktimeout = false;
bool checkcanvote = true;

extern string before;

void RegVoteCommand(){
    using ParamType = DynamicCommand::ParameterType;
    auto command = DynamicCommand::createCommand(Settings::command, cmdvote + endstr, CommandPermissionLevel::Any, { (CommandFlagValue)0x80 }, { (CommandFlagValue)1 });
    command->addOverload();
    command->setCallback([](DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>& results) {
        auto pl = origin.getPlayer();
        if(checkcanvote == false){
            return output.error(cooling);
        }
        else{
            if(pl->isPlayer() == true){
                auto player = (Player*)pl;
                VoteForm(player);
                return;
            }
            else{
                output.error(cmdplonly);
                return;
            }
        }
    });
    DynamicCommand::setup(std::move(command));
}

void RegCleanCommand(){
    using ParamType = DynamicCommand::ParameterType;
    auto command = DynamicCommand::createCommand(Settings::cleancommand, cmdclean + endstr, CommandPermissionLevel::GameMasters, { (CommandFlagValue)0x80 }, { (CommandFlagValue)1 });
    command->addOverload();
    command->setCallback([](DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>& results) {
        if(Settings::sendbroadcast == true){
            Global<Level>->broadcastText(before + opclean,TextType::RAW);
        }
        if(Settings::sendtoast == true){
            auto players = Global<Level>->getAllPlayers();
            if(players.size() >= 1){
                for(int tg = 0; tg <= players.size()-1; tg++){
                    players[tg]->sendToastPacket(notice,before + opclean);
                }
            }
        }
        _CleanerMain();
    });
    DynamicCommand::setup(std::move(command));
}

void ReloadCleaner(){
    Settings::LoadConfigFromJson(ConfigFile);
    LanguageSettings::LoadLanguageFromJson(LanguageFile);
}

void RegReloadCommand(){
    using ParamType = DynamicCommand::ParameterType;
    auto command = DynamicCommand::createCommand("reloadcleaner", reloadcmd + endstr, CommandPermissionLevel::GameMasters, { (CommandFlagValue)0x80 }, { (CommandFlagValue)1 });
    command->addOverload();
    command->setCallback([](DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>& results) {
        ReloadCleaner();
        return output.success(reloadsuccess);
    });
    DynamicCommand::setup(std::move(command));
}

void RegDespawnCommand(){
    using ParamType = DynamicCommand::ParameterType;
    auto command = DynamicCommand::createCommand(Settings::despawncmd, cmddespawn + endstr, CommandPermissionLevel::GameMasters, { (CommandFlagValue)0x80 }, { (CommandFlagValue)1 });
    command->mandatory("target",ParamType::Actor);
    command->addOverload({"target"});
    command->setCallback([](DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output, std::unordered_map<std::string, DynamicCommand::Result>& results) {
        auto ens = results["target"].get<std::vector<Actor*>>();
        if(ens.size() >= 1){
            for(int i = 0; i <= ens.size()-1 ; i++){
                ens[i]->despawn();
            }
            LoadLanguageFromJson(LanguageFile);
            
            return output.success(ReplaceStr(despawnsuccess,"{0}",std::to_string(ens.size())));
        }
        else return output.error(notarget);
    });
    DynamicCommand::setup(std::move(command));
}