#pragma once
#include <string>
#include <vector>

namespace Cleaner {
struct Config {
    int version = 2;

    std::string language = "zh_CN";

    struct basic {
        std::string Command       = "cleaner";
        int         Notice1       = 20;
        int         Notice2       = 5;
        bool        ConsoleLog    = true;
        bool        SendBroadcast = true;
        bool        SendToast     = true;
    } Basic;

    std::vector<std::string> IgnoreTags = {"ignore", "不清理"};

    struct auto_clean_count {
        bool Enabled      = true;
        int  TriggerCount = 900;
    } AutoCleanCount;

    struct auto_clean_tps {
        bool Enabled    = true;
        int  TriggerTPS = 15;
    } AutoCleanTPS;

    struct item_despawn {
        bool                     Enabled     = true;
        int                      DespawnTime = 6000;
        std::vector<std::string> WhiteList   = {"minecraft:elytra"};
    } ItemDespawn;

    struct Clean_Inanimate {
        bool                     Enabled   = true;
        std::vector<std::string> Blacklist = {
            "minecraft:xp_orb",
            "minecraft:arrow",
            "minecraft:fireball",
            "minecraft:small_fireball",
            "minecraft:wither_skull",
            "minecraft:wither_skull_dangerous",
            "minecraft:dragon_fireball"
        };
    } CleanInanimate;

    struct Clean_Item {
        bool                     Enabled    = true;
        int                      ExistTicks = 0;
        std::vector<std::string> Whitelist  = {
            "minecraft:netherite_helmet",   "minecraft:netherite_sword",    "minecraft:netherite_chestplate",
            "minecraft:diamond_helmet",     "minecraft:netherite_leggings", "minecraft:undyed_shulker_box",
            "minecraft:netherite_boots",    "minecraft:shulker_box",        "minecraft:elytra",
            "minecraft:dragon_egg",         "minecraft:nether_star",        "minecraft:diamond_sword",
            "minecraft:diamond_chestplate", "minecraft:diamond_boots",      "minecraft:diamond_leggings",
            "minecraft:diamond_hoe",        "minecraft:netherite_hoe",      "minecraft:diamond_axe",
            "minecraft:netherite_axe",      "minecraft:netherite_pickaxe",  "minecraft:diamond_pickaxe"
        };
    } CleanItem;

    struct Clean_Mobs {
        bool                     Enabled           = true;
        bool                     CleanMonstors     = true;
        bool                     CleanPeacefulMobs = false;
        bool                     EnableAutoExclude = true;
        std::vector<std::string> BlackList         = {"minecraft:guardian", "minecraft:zombie_pigman"};
        std::vector<std::string> Whitelist         = {
            "minecraft:ender_dragon",
            "minecraft:shulker",
            "minecraft:wither",
            "minecraft:elder_guardian",
            "minecraft:piglin_brute",
            "minecraft:ender_pearl",
            "minecraft:phantom"
        };
    } CleanMobs;

    struct Schedule_Clean {
        bool Enabled       = true;
        int  CleanInterval = 3600;
    } ScheduleClean;

    struct Vote_Clean {
        bool        Enabled          = true;
        std::string VoteCleanCommand = "voteclean";
        int         Cooldown         = 120;
        int         CheckDelay       = 30;
        int         Percentage       = 50;
    } VoteClean;
};
} // namespace Cleaner