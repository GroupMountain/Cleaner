#pragma once
#include "Global.h"

inline std::string defaultConfig = R"({
    "Basic": {
        "Language": "en_US",
        "ConsoleLog": true,
        "Command": "cleaner",
        "Notice1": 20,
        "Notice2": 5,
        "SendBroadcast": true
    },
    "AutoCleanCount": {
        "Enabled": true,
        "TriggerCount": 900
    },
    "AutoCleanTPS": {
        "Enabled": false,
        "TriggerTPS": 15
    },
    "ItemDespawn": {
        "Enabled": true,
        "DespawnTime": 6000,
        "WhiteList": [
            "minecraft:elytra"
        ]
    },
    "CleanInanimate": {
        "Blacklist": [
            "minecraft:xp_orb",
            "minecraft:arrow",
            "minecraft:fireball",
            "minecraft:small_fireball",
            "minecraft:wither_skull",
            "minecraft:wither_skull_dangerous",
            "minecraft:dragon_fireball"
        ],
        "Enabled": true
    },
    "CleanItem": {
        "Enabled": true,
        "ExistTicks": 0,
        "Whitelist": [
            "minecraft:netherite_helmet",
            "minecraft:netherite_sword",
            "minecraft:netherite_chestplate",
            "minecraft:diamond_helmet",
            "minecraft:netherite_leggings",
            "minecraft:undyed_shulker_box",
            "minecraft:netherite_boots",
            "minecraft:shulker_box",
            "minecraft:elytra",
            "minecraft:dragon_egg",
            "minecraft:nether_star",
            "minecraft:diamond_sword",
            "minecraft:diamond_chestplate",
            "minecraft:diamond_boots",
            "minecraft:diamond_leggings",
            "minecraft:diamond_hoe",
            "minecraft:netherite_hoe",
            "minecraft:diamond_axe",
            "minecraft:netherite_axe",
            "minecraft:netherite_pickaxe",
            "minecraft:diamond_pickaxe"
        ]
    },
    "CleanMobs": {
        "BlackList": [
            "minecraft:guardian",
            "minecraft:zombie_pigman"
        ],
        "CleanMonstors": true,
        "CleanPeacefulMobs": false,
        "EnableAutoExclude": true,
        "Enabled": true,
        "IgnoreTags": [
            "ignore",
            "不清理"
        ],
        "Whitelist": [
            "minecraft:ender_dragon",
            "minecraft:shulker",
            "minecraft:wither",
            "minecraft:elder_guardian",
            "minecraft:piglin_brute",
            "minecraft:ender_pearl",
            "minecraft:phantom"
        ]
    },
    "ScheduleClean": {
        "CleanInterval": 3600,
        "Enabled": true
        },    
    "VoteClean": {
        "CD": 120,
        "Delay": 30,
        "Enabled": true,
        "Percentage": 50,
        "VoteCleanCommand": "voteclean"
    }
})";