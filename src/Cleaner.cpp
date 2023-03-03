#include "setting.h"
#include "Global.h"
#include <llapi/mc/HashedString.hpp>
#include <llapi/mc/Mob.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Player.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <windows.h>

using namespace Settings;
using namespace LanguageSettings;

extern float Tick_per_minute;

bool ifautoclean;

string before = "§l§e[Cleaner]§r ";

void CleanerMain(){
    int k = 0;
    auto allentities = Level::getAllEntities(); 
    if(allentities.size() >= 1){
        for(Actor* en : allentities){
            if (en->isPlayer()) {
                continue;
            }
            bool ifignore = false;
            for (auto tag : IgnoreTags) {
                if (en->hasTag(tag)) {
                    ifignore = true;
                    break;
                }
            }
            if (ifignore = true) {
                continue;
            }
            if (en->isItemActor() == true) {
                if(enableitem == true){
                    auto item = (ItemActor*)en;
                    auto itemtype = item->getItemStack()->getTypeName();
                    if(!ItemList.count(itemtype)){
                        if (item->getLatestSpawnTime() >= ExistTicks) {
                            en->despawn();
                            k++;
                        }
                    }
                }
            }
            else if (en->isAlive()) {
                auto tname = en->getTypeName();
                if(enableinanimate == true){
                    if(InanimateList.count(tname)){
                        en->despawn();
                        k++;
                    }
                }
                if (enablemobs == true) {
                    auto mobtype = en->getTypeName();
                    if(!MobList.count(mobtype)){
                        if (en->hasFamily("monster")) {
                            if (cleanmonsters == true) {
                                if (mobblackonly == false) {
                                    if (Exclude == true) {
                                        auto ifa = en->canShowNameTag();
                                        auto ifb = en->isTame();
                                        auto ifc = en->isTrusting();
                                        if(ifa == false && ifb == false && ifc == false){
                                            en->despawn();
                                            k++;
                                        }
                                    }
                                    else{
                                        en->despawn();
                                        k++;
                                    }
                                }
                                else{
                                    if(MobBlackList.count(en->getTypeName())){
                                        if(Exclude == true){
                                            auto ifa = en->canShowNameTag();
                                            auto ifb = en->isTame();
                                            auto ifc = en->isTrusting();
                                            if(ifa == false && ifb == false && ifc == false){
                                                en->despawn();
                                                k++;
                                            }
                                        }
                                        else{
                                            en->despawn();
                                            k++;
                                        }
                                    }
                                }                                   
                            }
                        }
                        else{
                            if(cleanpeacefulmobs == true){
                                if(mobblackonly == false){
                                    if(Exclude == true){
                                        auto ifa = en->canShowNameTag();
                                        auto ifb = en->isTame();
                                        auto ifc = en->isTrusting();
                                        if(ifa == false && ifb == false && ifc == false){
                                            en->despawn();
                                            k++;
                                        }
                                    }
                                    else{
                                        en->despawn();
                                        k++;
                                    }
                                }
                                else{
                                    if(MobBlackList.count(en->getTypeName())){
                                        if(Exclude == true){
                                            auto ifa = en->canShowNameTag();
                                            auto ifb = en->isTame();
                                            auto ifc = en->isTrusting();
                                            if(ifa == false && ifb == false && ifc == false){
                                                en->despawn();
                                                k++;
                                            }
                                        }
                                        else{
                                            en->despawn();
                                            k++;
                                        }
                                    }
                                }             
                            }
                        }
                    }
                }
            }
            else{
                auto tname = en->getTypeName();
                if(enableinanimate == true){
                    if(InanimateList.count(tname)){
                        en->despawn();
                        k++;
                    }
                }
            }
        }    
        ifautoclean = true;
        LoadLanguageFromJson(LanguageFile);
        if(Settings::sendbroadcast == true){
            Level::broadcastText(before + ReplaceStr(output4,"{0}",std::to_string(k)),TextType::RAW);
        }
        if(Settings::sendtoast == true){
            auto players = Level::getAllPlayers();
            if(players.size() >= 1){
                for(int tg = 0; tg <= players.size()-1; tg++){
                    players[tg]->sendToastPacket(notice,before + ReplaceStr(output4,"{0}",std::to_string(k)));
                }
            }
        }
        if(Settings::logtoconsole == true){
            logger.info(ReplaceStr(output4,"{0}",std::to_string(k)));
        }
    }
    else{
        LoadLanguageFromJson(LanguageFile);
        if(Settings::sendbroadcast == true){
            Level::broadcastText(before + ReplaceStr(output4,"{0}","0"),TextType::RAW);
        }
        if(Settings::sendtoast == true){
            auto players = Level::getAllPlayers();
            if(players.size() >= 1){
                for(int tg = 0; tg <= players.size()-1; tg++){
                    players[tg]->sendToastPacket(notice,before + ReplaceStr(output4,"{0}","0"));
                }
            }
        }
        if(Settings::logtoconsole == true){
            logger.info(ReplaceStr(output4,"{0}","0"));
        }
    }
}

void Count1(){
    if(Settings::sendbroadcast == true){
        Level::broadcastText(before + ReplaceStr(output02,"{0}",std::to_string(pre2)),TextType::RAW);
    }
    if(Settings::logtoconsole == true){
        logger.info(ReplaceStr(output02,"{0}",std::to_string(pre2)));
    }
    if(Settings::sendtoast == true){
        auto players = Level::getAllPlayers();
        if(players.size() >= 1){
            for(int tga = 0; tga <= players.size()-1; tga++){
                players[tga]->sendToastPacket(notice,before + ReplaceStr(output02,"{0}",std::to_string(pre2)));
            }
        }
    }
}

void _CleanerMain(){
    LoadLanguageFromJson(LanguageFile);
    if(Settings::sendbroadcast == true){
        Level::broadcastText(before + ReplaceStr(output01,"{0}",std::to_string(pre1)),TextType::RAW);
    }
    if(Settings::logtoconsole == true){
        logger.info(ReplaceStr(output01,"{0}",std::to_string(pre1)));
    }
    if(Settings::sendtoast == true){
        auto players = Level::getAllPlayers();
        if(players.size() >= 1){
            for(int tg = 0; tg <= players.size()-1; tg++){
                players[tg]->sendToastPacket(notice,before + ReplaceStr(output01,"{0}",std::to_string(pre1)));
            }
        }
    }

    Schedule::delay([]{
        Count1();
    },(pre1-pre2)*20);

    Schedule::delay([]{
        CleanerMain();
    },pre1*20);
}

void Cleaner(){
    Schedule::repeat([]{
        if(Settings::enableschdule == true){
            _CleanerMain();
        } 
    },20*CleanInterval);
}

void _AutoClean() {
    int j = 0;
    auto allentities = Level::getAllEntities(); 
    if(allentities.size() >= 1){
        for(Actor* en : allentities){
            if (en->isPlayer()) {
                continue;
            }
            bool ifignore = false;
            for (auto tag : IgnoreTags) {
                if (en->hasTag(tag)) {
                    ifignore = true;
                    break;
                }
            }
            if (ifignore = true) {
                continue;
            }
            if (en->isItemActor() == true) {
                if(enableitem == true){
                    auto item = (ItemActor*)en;
                    auto itemtype = item->getItemStack()->getTypeName();
                    if(!ItemList.count(itemtype)){
                        if (item->getLatestSpawnTime() >= ExistTicks) {
                            j++;
                        }
                    }
                }
            }
            else if (en->isAlive()) {
                auto tname = en->getTypeName();
                if(enableinanimate == true){
                    if(InanimateList.count(tname)){
                        j++;
                    }
                }
                if (enablemobs == true) {
                    auto mobtype = en->getTypeName();
                    if(!MobList.count(mobtype)){
                        if (en->hasFamily("monster")) {
                            if (cleanmonsters == true) {
                                if (mobblackonly == false) {
                                    if (Exclude == true) {
                                        auto ifa = en->canShowNameTag();
                                        auto ifb = en->isTame();
                                        auto ifc = en->isTrusting();
                                        if(ifa == false && ifb == false && ifc == false){
                                            j++;
                                        }
                                    }
                                    else{
                                        j++;
                                    }
                                }
                                else{
                                    if(MobBlackList.count(en->getTypeName())){
                                        if(Exclude == true){
                                            auto ifa = en->canShowNameTag();
                                            auto ifb = en->isTame();
                                            auto ifc = en->isTrusting();
                                            if(ifa == false && ifb == false && ifc == false){
                                                j++;
                                            }
                                        }
                                        else{
                                            j++;
                                        }
                                    }
                                }                                   
                            }
                        }
                        else{
                            if(cleanpeacefulmobs == true){
                                if(mobblackonly == false){
                                    if(Exclude == true){
                                        auto ifa = en->canShowNameTag();
                                        auto ifb = en->isTame();
                                        auto ifc = en->isTrusting();
                                        if(ifa == false && ifb == false && ifc == false){
                                            j++;
                                        }
                                    }
                                    else{
                                        j++;
                                    }
                                }
                                else{
                                    if(MobBlackList.count(en->getTypeName())){
                                        if(Exclude == true){
                                            auto ifa = en->canShowNameTag();
                                            auto ifb = en->isTame();
                                            auto ifc = en->isTrusting();
                                            if(ifa == false && ifb == false && ifc == false){
                                                j++;
                                            }
                                        }
                                        else{
                                            j++;
                                        }
                                    }
                                }             
                            }
                        }
                    }
                }
            }
            else{
                auto tname = en->getTypeName();
                if(enableinanimate == true){
                    if(InanimateList.count(tname)){
                        j++;
                    }
                }
            }
        }    
        if (j >= Settings::triggercount) {
            ifautoclean = false;     
            LoadLanguageFromJson(LanguageFile);
            if(Settings::logtoconsole == true){
                logger.warn(ReplaceStr(trigger1,"{0}",std::to_string(j)));
            }
            if(Settings::sendtoast == true){
                auto players = Level::getAllPlayers();
                for(int tg = 0; tg <= players.size()-1; tg++){
                    players[tg]->sendToastPacket(notice,before + ReplaceStr(trigger1,"{0}",std::to_string(j)));
                }
            }
            if(Settings::sendbroadcast == true){
                Level::broadcastText(before + ReplaceStr(trigger1,"{0}",std::to_string(j)),TextType::RAW);
            } 
            _CleanerMain(); 
        }        
    }
}

void AutoClean(){
    ifautoclean = true;
    Schedule::repeat([]{
        if(ifautoclean == true && Settings::enableautoclean == true){
            _AutoClean();
        }   
    },160);
}

void TPSClean(){
    Schedule::repeat([]{
		if(enabletpsclean == true && Tick_per_minute <= triggertps){
            LoadLanguageFromJson(LanguageFile);
            if(Settings::logtoconsole == true){
                logger.warn(ReplaceStr(triggertpsclean,"{0}",std::to_string(Tick_per_minute)));
            }
            if(Settings::sendtoast == true){
                auto players = Level::getAllPlayers();
                for(int tg = 0; tg <= players.size()-1; tg++){
                    players[tg]->sendToastPacket(notice,before + ReplaceStr(triggertpsclean,"{0}",std::to_string(Tick_per_minute)));
                }
            }
            if(Settings::sendbroadcast == true){
                Level::broadcastText(before + ReplaceStr(triggertpsclean,"{0}",std::to_string(Tick_per_minute)),TextType::RAW);
            } 
            _CleanerMain();
        }
	},300);
}