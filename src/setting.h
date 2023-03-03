#pragma once

#include <llapi/utils/FileHelper.h>
#include <Nlohmann/json.hpp>

namespace Settings {
    extern int CleanInterval;
    extern bool logtoconsole;
    extern bool enableitem;
    extern bool enablevote;
    extern bool sendtoast;
    extern int cd;
    extern int votedelay;
    extern bool enablemobs;
    extern bool cleanpeacefulmobs;
    extern bool cleanmonsters;
    extern bool Exclude;
    extern bool enableinanimate;
    extern int percentage;
    extern int triggercount;
    extern bool enableautoclean;
    extern bool enableschdule;
    extern bool sendbroadcast;
    extern int percentage;
    extern int pre1;
    extern int pre2;
    extern bool enabletpsclean;
    extern bool regtpscmd;
    extern int triggertps;
    extern int ExistTicks;
    extern bool regdespawncmd;
    extern string despawncmd;
    extern string tpscmd;
    extern string command;
    extern string cleancommand;
    extern std::unordered_set<string> MobBlackList;
    extern bool mobblackonly;
    extern std::unordered_set<string> MobList;
    extern std::unordered_set<string> ItemList;
    extern std::unordered_set<string> InanimateList;
    extern std::unordered_set<string> IgnoreTags;

    extern nlohmann::json globaljson();
    void initjson(nlohmann::json json);
    void WriteDefaultConfig(const std::string& fileName);
    void LoadConfigFromJson(const std::string& fileName);
    void reloadJson(const std::string& fileName);
}

namespace LanguageSettings {
    extern string cmdplonly;
    extern string output01;
    extern string output02;
    extern string output4;
    extern string cmdvote;
    extern string cmdclean;
    extern string opclean;
    extern string cooling;
    extern string votesuccess;
    extern string votefail;
    extern string guitit;
    extern string guiqus;
    extern string sendok;
    extern string sendno;
    extern string sendcancel;
    extern string reask;
    extern string reok;
    extern string reno;
    extern string reout;
    extern string reac;
    extern string rede;
    extern string notice;
    extern string trigger1;
    extern string reloadcmd;
    extern string reloadsuccess;
    extern string tpscmd;
    extern string tpscmdout;
    extern string triggertpsclean;
    extern string cmddespawn;
    extern string notarget;
    extern string despawnsuccess;
    extern nlohmann::json globaljson();
    void initjson(nlohmann::json json);
    void WriteDefaultLanguage(const std::string& fileName);
    void LoadLanguageFromJson(const std::string& fileName);
    void reloadJson(const std::string& fileName);
}