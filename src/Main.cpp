#include "version.h"
#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/ServerAPI.h>

#include "Global.h"
#include "setting.h"

#include <llapi/RemoteCallAPI.h>

using namespace std;

extern Logger logger;
extern void Cleaner();
extern void AutoClean();
extern void RegVoteCommand();
extern void RegCleanCommand();
extern void RegReloadCommand();
extern void ReloadCleaner();

extern void _CleanerMain();
extern void CleanerMain();

extern void CaculateTPS();
extern void RegTPSCommand();
extern void TPSClean();
extern void RegDespawnCommand();

extern unsigned short Tick_per_second;
extern float Tick_per_minute;

void Despawn(Actor* entity){
    entity->despawn();
}

void PluginInit(){
    cout << R"(                                                                      )" << endl;
    cout << R"(           ________  _                                                )" << endl;
    cout << R"(          /  ____  \| |                                               )" << endl;
    cout << R"(          | |    |_|| |                                               )" << endl;
    cout << R"(          | |       | |  ____   ______ ____   ____   _____            )" << endl;
    cout << R"(          | |     _ | | / __ \ / __  // __ \ / __ \ / ___/            )" << endl;
    cout << R"(          | |____| || |/ /___// /_/ // / / // /___// /                )" << endl;
    cout << R"(          \________/|_|\____/ \____//_/ /_/ \____//_/                 )" << endl;
    cout << R"(                                                                      )" << endl;
    cout << R"(         -------- A Powerful Entity Cleanup Plugin --------           )" << endl;
    cout << R"(                                                                      )" << endl;
    cout << R"(                                                                      )" << endl;
    if(GetSystemDefaultLangID() == 0x0804){
        logger.info("当前版本 {}.{}.{} ，支持 BDS - 1.19.6x ，版本协议 {}",PLUGIN_VERSION_MAJOR,PLUGIN_VERSION_MINOR,PLUGIN_VERSION_REVISION,TARGET_BDS_PROTOCOL_VERSION);
        logger.info("插件作者 Tsubasa6848");
        logger.info("正在加载 Cleaner ...");
        logger.info("这个插件是免费的，如果你花钱了，请立即差评并要求RNM退钱。");
    }
    else{
        logger.info("Current Version: {}.{}.{} , support BDS - 1.19.6x , protocal version {}",PLUGIN_VERSION_MAJOR,PLUGIN_VERSION_MINOR,PLUGIN_VERSION_REVISION,TARGET_BDS_PROTOCOL_VERSION);
        logger.info("Plugin Author: Tsubasa6848");
        logger.info("Loading Cleaner ...");
        logger.info("This plugin is free, and if you paid for it, you must have been scammed.");
    }

    Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent) {
        CaculateTPS();
        RegCleanCommand();
        Cleaner();
        if(Settings::enablevote == true){
            RegVoteCommand();
        }
        if(Settings::regtpscmd == true){
            RegTPSCommand();
        }
        if(Settings::regdespawncmd == true){
            RegDespawnCommand();
        }
        if(Settings::enableautoclean == true){
            AutoClean();
        }
        if(Settings::enabletpsclean == true){
            TPSClean();
        }    
        RegReloadCommand();
        return true;
    });  
    RemoteCall::exportAs("Cleaner", "ReloadCleaner", []() -> void { return ReloadCleaner(); });
    RemoteCall::exportAs("Cleaner", "Clean", []() -> void { return _CleanerMain(); });
    RemoteCall::exportAs("Cleaner", "CleanMain", []() -> void { return CleanerMain(); });
    RemoteCall::exportAs("Cleaner", "GetCurrentTPS", []() -> unsigned short { return Tick_per_second; });
	RemoteCall::exportAs("Cleaner", "GetAverageTPS", []() -> float { return Tick_per_minute; });
    RemoteCall::exportAs("Cleaner", "Despawn", [](Actor* entity) -> void { return Despawn(entity); });
}
