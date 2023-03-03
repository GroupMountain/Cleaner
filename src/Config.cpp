#include "setting.h"
#include "Global.h"
#include <llapi/LoggerAPI.h>

string readme = "《用户最终许可协议(EULA)》/插件使用须知\n欢迎使用本插件。\n插件内一切功能均可以自定义开关！\n未经许可，本插件不接受任何整合、转载和二次开发。\n本插件目前只发布在MineBBS上面。\n如果造成崩溃，请反复排除是否为本插件问题，如果是本插件造成的，请提交崩溃日志。\n使用过程中难免会有漏洞，请在MineBBS插件的讨论区反馈，作者会尽快修复。请分清楚评论区和讨论区。\n对于以上条款有任何异议，请停止使用并卸载本插件。\n如果同意使用本插件，请在../plugins/Cleaner/EULA.txt 里面进行确认。\n把EULA=false改为EULA=true并保存即可。\n\n\nEnd User Licence Agreement(EULA)/Instruction for Plugin Advancements\nWelcome to use this plugin.\nDo NOT integrate, reproduce or secondary develop this plugin without permission.\nThis plugin only released on MineBBS, see www.minebbs.com\nIf a crash is caused, please repeatedly troubleshoot whether the problem is with the plugin, and if it is caused by the plugin, please submit the crash log.\nThere will inevitably be bugs in the process of use, please give feedback in the discussion area of the MineBBS plugin, and the author will fix it as soon as possible. Please distinguish between comment areas and discussion areas.\nIf you have any objections to the above terms, please stop using and uninstall this plugin.\nIf you agree to use this plugin, please confirm it in   ../plugins/Cleaner/EULA.txt .\nchange  EULA=false  to  EULA=true  and save it.";

void loadConfig() { 
    if (!std::filesystem::exists("plugins/Cleaner")){
        std::filesystem::create_directories("plugins/Cleaner");	
    }
    if (!std::filesystem::exists("./plugins/Cleaner/README.md")){
        WriteAllFile("plugins/Cleaner/README.md",readme,false);
        if(GetSystemDefaultLangID() == 0x0804){
            logger.warn("插件说明 README.md 不存在，已自动创建！");
        }
        else{
            logger.warn("README.md does not exist, created automatically.");
        }
    }
    if (!std::filesystem::exists("./plugins/Cleaner/EULA.txt")){
        WriteAllFile("plugins/Cleaner/EULA.txt","EULA=false",false);
        if(GetSystemDefaultLangID() == 0x0804){
            logger.warn("插件 EULA 协议不存在，已自动创建！");
        }
        else{
            logger.warn("EULA does not exist, created automatically.");
        }
    }
    if (std::filesystem::exists(ConfigFile)) {
        Settings::LoadConfigFromJson(ConfigFile);
    }
    else {
        if(GetSystemDefaultLangID() == 0x0804){
            logger.warn("配置文件不存在，已自动创建！");
        }
        else{
            logger.warn("Config file does not exist, created automatically.");
        }
        Settings::WriteDefaultConfig(ConfigFile);
    }
}