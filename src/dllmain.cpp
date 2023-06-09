#include <llapi/LoggerAPI.h>
#include <llapi/ServerAPI.h>
#include "Global.h"
#include "version.h"

#include <stdio.h>
#include <WinNls.h>

extern void loadConfig();
extern void loadLanguage();

void PluginInit();

Logger logger(PLUGIN_NAME);

void CheckEULA(){
    auto readEULA = ReadAllFile("plugins/Cleaner/EULA.txt");
    auto EULA = ReplaceStr(readEULA.value()," ","");
    if(EULA == "EULA=true"){
        PluginInit();
    }
    else{
        if(GetSystemDefaultLangID() == 0x0804){
            if(MessageBox(NULL,TEXT("使用Cleaner插件将视为你同意本插件使用协议（EULA）\n\n你是否同意使用Cleaner插件？"),TEXT("Cleaner"),MB_DEFBUTTON2 | MB_ICONQUESTION | MB_YESNOCANCEL) == 6){
                WriteAllFile("plugins/Cleaner/EULA.txt","EULA=true");
                PluginInit();
            }
            else{
                Logger().fatal("无法加载插件 Cleaner！\n                未同意插件 EULA 协议或插件 EULA 协议配置异常。\n                请同意插件 EULA 协议后再重启服务器，具体请参见插件 README.md\n                如果无法接受插件 EULA 协议，请卸载本插件！");
            }
        }  
        else{
            if(MessageBox(NULL,TEXT("By using the plugin Cleaner, you must agree plugin EULA.\n\nDo you agree to use Cleaner?"),TEXT("Cleaner"),MB_YESNOCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION) == 6){
                WriteAllFile("plugins/Cleaner/EULA.txt","EULA=true");
                PluginInit();
            }
            else{
                Logger().fatal("Fail to Load Cleaner!\n                Please accept the EULA or check the EULA settings.\n                Restart the server after you accepted the EULA. Pleade read README.md\n                If you can`t accept the EULA, please uninstall the plugin.");
            }
        } 
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ll::registerPlugin(
            PLUGIN_NAME,
            PLUGIN_INTRODUCTION,
            ll::Version(PLUGIN_VERSION_MAJOR, PLUGIN_VERSION_MINOR, PLUGIN_VERSION_REVISION, PLUGIN_LLVERSION_STATUS),
            std::map<std::string, std::string>{
                {"Author", PLUGIN_AUTHOR},
            });
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C"
{
    _declspec(dllexport) void onPostInit()
    {
        std::ios::sync_with_stdio(false);
        loadConfig();
        loadLanguage();
        CheckEULA();
    }
}