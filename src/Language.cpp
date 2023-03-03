#include <llapi/utils/FileHelper.h>
#include <Nlohmann/json.hpp>
#include "Global.h"
#include "setting.h"

#define TRJ(key1,key2,val)                                \
if (json.find(key1) != json.end()) {                      \
    if (json.at(key1).find(key2) != json.at(key1).end()) {\
    const nlohmann::json& out2 = json.at(key1).at(key2);  \
         out2.get_to(val);}}                              \

void loadLanguage() {	
    if (std::filesystem::exists(LanguageFile)) {
        try {
            LanguageSettings::LoadLanguageFromJson(LanguageFile);
        }
        catch (std::exception& e) {
            if(GetSystemDefaultLangID() == 0x0804){
                logger.error("未知错误： {}\n正在删除语言文件...", e.what());
            }
            else{
                logger.error("Unknown Error: {}\nRemoving language file...", e.what());
            }
            remove(LanguageFile);
            exit(1);
        }
        catch (...) {
            if(GetSystemDefaultLangID() == 0x0804){
                logger.error("语言文件异常！正在删除语言文件...");
            }
            else{
                logger.error("Invalid Language file! Removing language file...");
            }
            remove(LanguageFile);
            exit(1);
        }
    } 
    else {
        if(GetSystemDefaultLangID() == 0x0804){
            logger.warn("语言文件不存在，已自动创建！");
        }
        else{
            logger.warn("Language file does not exist, created automatically.");
        }
        LanguageSettings::WriteDefaultLanguage(LanguageFile);
    }
}

namespace LanguageSettings {
    string cmdplonly = "该命令只能由玩家执行！";
    string output01 = "系统将在 {0} 秒后自动清理服务器实体！";
    string output02 = "请注意！系统将在 {0} 秒后自动清理服务器实体！";
    string output4 = "清理完成！本次总共清理了 {0} 个实体。";
    string cmdvote = "发起实体清理投票。";
    string cmdclean = "清理服务器实体。";
    string opclean = "管理员启用了服务器服务器实体清理。";
    string cooling = "投票清理冷却...";
    string votesuccess = "投票清理成功！";
    string votefail = "投票清理失败！玩家同意比例未达标！";
    string guitit = "投票清理";
    string guiqus = "你是否要发起投票清理？";
    string sendok = "发起投票";
    string sendno = "我再想想";
    string sendcancel = "投票已取消！";
    string reask = "你是否同意现在清理服务器实体？";
    string reok = "同意";
    string reno = "拒绝";
    string reout = "投票已过期！";
    string reac = "你已同意实体清理。";
    string rede = "你已拒绝实体清理。";
    string notice = "通知";
    string trigger1 = "检测到服务器实体过多！！当前服务器存在 {0} 个可清理实体，已启动自动清理程序。";
    string reloadcmd = "热重载Cleaner插件";
    string reloadsuccess = "已重载Cleaner插件,部分配置可能需要重启服务器才能生效。";
    string tpscmd = "查询服务器TPS";
    string tpscmdout = "当前服务器实时TPS {1} §r，平均TPS {2}";
    string triggertpsclean = "当前服务器TPS过低！！服务器平均TPS {0}\n系统已启动清理程序。";
    string cmddespawn = "通过Despawn清除实体";
    string notarget = "没有与选择器匹配的目标";
    string despawnsuccess = "已成功通过Despawn清除了 {0} 个实体";

    nlohmann::json globaljson() {
        nlohmann::json json;
        json["Language"]["command.error.player_only"] = cmdplonly;
        json["Language"]["output.count1"] = output01;
        json["Language"]["output.count2"] = output02;
        json["Language"]["output.finish"] = output4;
        json["Language"]["command.voteclean"] = cmdvote;
        json["Language"]["command.clean"] = cmdclean;
        json["Language"]["command.reload"] = reloadcmd;
        json["Language"]["command.despawn"] = cmddespawn;
        json["Language"]["command.error.notarget"] = notarget;
        json["Language"]["command.despawn.success"] = despawnsuccess;
        json["Language"]["output.reload"] = reloadsuccess;
        json["Language"]["output.opclean"] = opclean;
        json["Language"]["vote.cd"] = cooling;
        json["Language"]["vote.succeed"] = votesuccess;
        json["Language"]["vote.title"] = guitit;
        json["Language"]["vote.confirm.subtitle"] = guiqus;
        json["Language"]["vote.confirm.ok"] = sendok;
        json["Language"]["vote.confirm.no"] = sendno;
        json["Language"]["vote.confirm.cancel"] = sendcancel;
        json["Language"]["vote.gui.subtitle"] = reask;
        json["Language"]["vote.gui.ok"] = reok;
        json["Language"]["vote.gui.no"] = reno;
        json["Language"]["vote.gui.timeout"] = reout;
        json["Language"]["vote.gui.accept"] = reac;
        json["Language"]["vote.gui.deny"] = rede;
        json["Language"]["vote.toast.notice"] = notice;
        json["Language"]["output.triggerautoclean"] = trigger1;
        json["Language"]["command.tps"] = tpscmd;
        json["Language"]["command.tps.output"] = tpscmdout;
        json["Language"]["output.triggerautocleantps"] = triggertpsclean;

        return json;
    }

    void initjson(nlohmann::json json) {
        TRJ("Language","command.error.player_only",cmdplonly);
        TRJ("Language","output.count1",output01);
        TRJ("Language","output.count2",output02);
        TRJ("Language","output.finish",output4);
        TRJ("Language","command.voteclean",cmdvote);
        TRJ("Language","command.clean",cmdclean);
        TRJ("Language","command.reload",reloadcmd);
        TRJ("Language","output.reload",reloadsuccess);
        TRJ("Language","output.opclean",opclean);
        TRJ("Language","vote.cd",cooling);
        TRJ("Language","vote.succeed",votesuccess);
        TRJ("Language","vote.fail",votefail);
        TRJ("Language","vote.title",guitit);
        TRJ("Language","vote.confirm.subtitle",guiqus);
        TRJ("Language","vote.confirm.ok",sendok);
        TRJ("Language","vote.confirm.no",sendno);
        TRJ("Language","vote.confirm.cancel",sendcancel);
        TRJ("Language","vote.gui.subtitle",reask);
        TRJ("Language","vote.gui.ok",reok);
        TRJ("Language","vote.gui.no",reno);
        TRJ("Language","vote.gui.timeout",reout);
        TRJ("Language","vote.gui.accept",reac);
        TRJ("Language","vote.gui.deny",rede);
        TRJ("Language","vote.toast.notice",notice);
        TRJ("Language","output.triggerautoclean",trigger1);
        TRJ("Language","command.tps",tpscmd);
        TRJ("Language","command.tps.output",tpscmdout);
        TRJ("Language","output.triggerautocleantps",triggertpsclean);
        TRJ("Language","command.despawn",cmddespawn);
        TRJ("Language","command.error.notarget",notarget);
        TRJ("Language","command.despawn.success",despawnsuccess);
    }

    void WriteDefaultLanguage(const std::string& fileName) {
        std::ofstream file(fileName);
        if (!file.is_open()) {
            if(GetSystemDefaultLangID() == 0x0804){
                logger.warn("无法打开语言文件！");
            }
            else{
                logger.warn("Fail to open language file!");
            }
            return;
        }
        auto json = globaljson();
        file << json.dump(4);
        file.close();
    }

    void LoadLanguageFromJson(const std::string& fileName) {
        std::ifstream file(fileName);
        if (!file.is_open()) {
            if(GetSystemDefaultLangID() == 0x0804){
                logger.warn("无法打开语言文件！");
            }
            else{
                logger.warn("Fail to open language file!");
            }
            return;
        }
        nlohmann::json json;
        file >> json;
        file.close();
        initjson(json);
        WriteDefaultLanguage(fileName);
    }
    void reloadJson(const std::string& fileName) {
        std::ofstream file(fileName);
        if (file){
            file << globaljson().dump(4);
        }
        else{
            if(GetSystemDefaultLangID() == 0x0804){
                logger.warn("无法重载语言文件！");
            }
            else{
                logger.warn("Fail to reload language file!");
            }
        }
        file.close();
    }
} 