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
            if (ifignore == true) {
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
        auto opt = output4;
        ReplaceStr(opt, "{0}", std::to_string(k));
        if (Settings::sendbroadcast == true) {
            Level::broadcastText(before + opt, TextType::RAW);
        }
        if (Settings::sendtoast == true) {
            auto players = Level::getAllPlayers();
            if(players.size() >= 1){
                for(int tg = 0; tg <= players.size()-1; tg++){
                    players[tg]->sendToastPacket(notice,before + opt);
                }
            }
        }
        if (Settings::logtoconsole == true) {
            logger.info(opt);
        }
    }
    else{
        auto opt = output4;
        ReplaceStr(opt, "{0}", "0");
        if(Settings::sendbroadcast == true){
            Level::broadcastText(before + opt, TextType::RAW);
        }
        if(Settings::sendtoast == true){
            auto players = Level::getAllPlayers();
            if(players.size() >= 1){
                for(int tg = 0; tg <= players.size()-1; tg++){
                    players[tg]->sendToastPacket(notice,before + opt);
                }
            }
        }
        if(Settings::logtoconsole == true){
            logger.info(opt);
        }
    }
}

void Count1(){
    auto opt = output02;
    ReplaceStr(opt,"{0}",std::to_string(pre2));
    if(Settings::sendbroadcast == true){
        Level::broadcastText(before + opt, TextType::RAW);
    }
    if(Settings::logtoconsole == true){
        logger.info(opt);
    }
    if(Settings::sendtoast == true){
        auto players = Level::getAllPlayers();
        if(players.size() >= 1){
            for(int tga = 0; tga <= players.size()-1; tga++){
                players[tga]->sendToastPacket(notice,before + opt);
            }
        }
    }
}

void _CleanerMain(){
    auto opt = output01;
    ReplaceStr(opt,"{0}", std::to_string(pre1));
    if (Settings::sendbroadcast == true) {
        Level::broadcastText(before + opt, TextType::RAW);
    }
    if (Settings::logtoconsole == true) {
        logger.info(opt);
    }
    if (Settings::sendtoast == true) {
        auto players = Level::getAllPlayers();
        if(players.size() >= 1){
            for(int tg = 0; tg <= players.size()-1; tg++){
                players[tg]->sendToastPacket(notice,before + opt);
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
            if (ifignore == true) {
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
            auto opt = trigger1;
            ReplaceStr(opt,"{0}", std::to_string(j));
            if(Settings::logtoconsole == true){
                logger.warn(opt);
            }
            if(Settings::sendtoast == true){
                auto players = Level::getAllPlayers();
                for(int tg = 0; tg <= players.size()-1; tg++){
                    players[tg]->sendToastPacket(notice,before + opt);
                }
            }
            if(Settings::sendbroadcast == true){
                Level::broadcastText(before + opt, TextType::RAW);
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
            auto opt = triggertpsclean;
            ReplaceStr(opt,"{0}", std::to_string(Tick_per_minute));
            if(Settings::logtoconsole == true){
                logger.warn(opt);
            }
            if(Settings::sendtoast == true){
                auto players = Level::getAllPlayers();
                for(int tg = 0; tg <= players.size()-1; tg++){
                    players[tg]->sendToastPacket(notice,before + opt);
                }
            }
            if(Settings::sendbroadcast == true){
                Level::broadcastText(before + opt, TextType::RAW);
            } 
            _CleanerMain();
        }
	},300);
}