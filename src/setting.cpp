#include <llapi/utils/FileHelper.h>
#include <Nlohmann/json.hpp>
#include "Global.h"

#define TRJ(key1, key2, val)                                     \
    if (json.find(key1) != json.end())                           \
    {                                                            \
        if (json.at(key1).find(key2) != json.at(key1).end())     \
        {                                                        \
            const nlohmann::json &out2 = json.at(key1).at(key2); \
            out2.get_to(val);                                    \
        }                                                        \
    }

namespace Settings
{
    int CleanInterval = 300;
    int cd = 120;
    int votedelay = 30;
    bool sendtoast = true;
    bool logtoconsole = true;
    bool enableitem = true;
    bool enablevote = true;
    bool enablemobs = true;
    bool cleanpeacefulmobs = false;
    bool cleanmonsters = true;
    bool Exclude = true;
    bool enableinanimate = true;
    bool enableautoclean = true;
    bool enableschdule = true;
    bool sendbroadcast = true;
    int triggercount = 500;
    int percentage = 50;
    int pre1 = 20;
    int pre2 = 5;
    bool enabletpsclean = false;
    bool regtpscmd = false;
    bool regdespawncmd = false;
    int triggertps = 5;
    bool mobblackonly = false;
    int ExistTicks = 0;
    string despawncmd = "despawn";
    string tpscmd = "tps";
    string command = "voteclean";
    string cleancommand = "clean";
    std::unordered_set<string> MobList = {
        "minecraft:ender_dragon",
        "minecraft:shulker",
        "minecraft:elder_guardian",
        "minecraft:piglin_brute"
    };
    std::unordered_set<string> MobBlackList = {
        "minecraft:guardian",
        "minecraft:zombie_pigman"
    };
    std::unordered_set<string> ItemList = {
        "minecraft:netherite_helmet",
        "minecraft:netherite_chestplate",
        "minecraft:netherite_leggings",
        "minecraft:netherite_boots",
        "minecraft:netherite_sword",
        "minecraft:shulker_box",
        "minecraft:undyed_shulker_box",
        "minecraft:elytra",
        "minecraft:dragon_egg",
        "minecraft:nether_star"};
    std::unordered_set<string> InanimateList = {
        "minecraft:xp_orb",
        "minecraft:arrow",
        "minecraft:fireball",
        "minecraft:small_fireball",
        "minecraft:wither_skull",
        "minecraft:wither_skull_dangerous",
        "minecraft:dragon_fireball"
    };
    std::unordered_set<string> IgnoreTags = {
        "ignore",
        "不清理"
    };

    nlohmann::json globaljson()
    {
        nlohmann::json json;
        json["Basic"]["SendBroadcast"] = sendbroadcast;
        json["Basic"]["ConsoleLog"] = logtoconsole;
        json["Basic"]["CleanCommand"] = cleancommand;
        json["Basic"]["SendToast"] = sendtoast;
        json["Basic"]["RegisterTPSCommand"] = regtpscmd;
        json["Basic"]["TPSCommand"] = tpscmd;
        json["Basic"]["RegisterDespawnCommand"] = regdespawncmd;
        json["Basic"]["DespawnCommand"] = despawncmd;
        json["ScheduleClean"]["CleanInterval"] = CleanInterval;
        json["ScheduleClean"]["Enabled"] = enableschdule;
        json["VoteClean"]["Enabled"] = enablevote;
        json["VoteClean"]["VoteCleanCommand"] = command;
        json["VoteClean"]["Percentage"] = percentage;
        json["VoteClean"]["CD"] = cd;
        json["VoteClean"]["Delay"] = votedelay;
        json["CleanItem"]["Enabled"] = enableitem;
        json["CleanItem"]["Whitelist"] = ItemList;
        json["CleanItem"]["ExistTicks"] = ExistTicks;
        json["CleanMobs"]["Enabled"] = enablemobs;
        json["CleanMobs"]["CleanPeacefulMobs"] = cleanpeacefulmobs;
        json["CleanMobs"]["CleanMonstors"] = cleanmonsters;
        json["CleanMobs"]["EnableAutoExclude"] = Exclude;
        json["CleanMobs"]["Whitelist"] = MobList;
        json["CleanMobs"]["BlackListOnly"] = mobblackonly;
        json["CleanMobs"]["BlackList"] = MobBlackList;
        json["CleanMobs"]["IgnoreTags"] = IgnoreTags;
        json["CleanInanimate"]["Enabled"] = enableinanimate;
        json["CleanInanimate"]["Blacklist"] = InanimateList;
        json["AutoCleanCount"]["Enabled"] = enableautoclean;
        json["AutoCleanCount"]["TriggerCount"] = triggercount;
        json["AutoCleanTPS"]["TriggerTPS"] = triggertps;
        json["AutoCleanTPS"]["Enabled"] = enabletpsclean;
        json["Basic"]["Notice1"] = pre1;
        json["Basic"]["Notice2"] = pre2;
        
        return json;
    }

    void initjson(nlohmann::json json)
    {
        TRJ("Basic","SendBroadcast",sendbroadcast);
        TRJ("Basic", "ConsoleLog", logtoconsole);
        TRJ("Basic", "CleanCommand", cleancommand);
        TRJ("Basic", "SendToast", sendtoast);
        TRJ("Basic","RegisterTPSCommand",regtpscmd);
        TRJ("Basic","TPSCommand",tpscmd);
        TRJ("ScheduleClean", "CleanInterval", CleanInterval);
        TRJ("ScheduleClean", "Enabled", enableschdule);
        TRJ("VoteClean", "Enabled", enablevote);
        TRJ("VoteClean", "Command", command);
        TRJ("VoteClean", "Percentage", percentage);
        TRJ("VoteClean", "CD", cd);
        TRJ("VoteClean", "Delay", votedelay);
        TRJ("CleanItem", "Enabled", enableitem);
        TRJ("CleanItem", "Whitelist", ItemList);
        TRJ("CleanItem","ExistTicks", ExistTicks);
        TRJ("CleanMobs", "Enabled", enablemobs);
        TRJ("CleanMobs", "CleanPeacefulMobs", cleanpeacefulmobs);
        TRJ("CleanMobs", "CleanMonstors", cleanmonsters);
        TRJ("CleanMobs", "EnableAutoExclude", Exclude);
        TRJ("CleanMobs", "Whitelist", MobList);
        TRJ("CleanMobs","BlackListOnly",mobblackonly);
        TRJ("CleanMobs","BlackList",MobBlackList);
        TRJ("CleanInanimate", "Enabled", enableinanimate);
        TRJ("CleanInanimate", "Blacklist", InanimateList);
        TRJ("AutoCleanCount", "Enabled", enableautoclean);
        TRJ("AutoCleanCount", "TriggerCount", triggercount);
        TRJ("AutoCleanTPS", "Enabled", enabletpsclean);
        TRJ("AutoCleanTPS", "TriggerTPS", triggertps);
        TRJ("Basic","Notice1",pre1);
        TRJ("Basic","Notice2",pre2);
        TRJ("Basic","RegisterDespawnCommand",regdespawncmd);
        TRJ("Basic","DespawnCommand",despawncmd);
        TRJ("CleanMobs","IgnoreTags", IgnoreTags);
    }

    void WriteDefaultConfig(const std::string &fileName)
    {
        std::ofstream file(fileName);
        if (!file.is_open())
        {
            logger.error("无法打开配置文件");
            return;
        }
        auto json = globaljson();
        file << json.dump(4);
        file.close();
    }

    void LoadConfigFromJson(const std::string &fileName)
    {
        std::ifstream file(fileName);
        if (!file.is_open())
        {
            logger.error("无法打开配置文件");
            return;
        }
        nlohmann::json json;
        file >> json;
        file.close();
        initjson(json);
        WriteDefaultConfig(fileName);
    }
    void reloadJson(const std::string &fileName)
    {
        std::ofstream file(fileName);
        if (file)
        {
            file << globaljson().dump(4);
        }
        else
        {
            logger.error("配置文件异常！");
        }
        file.close();
    }
}