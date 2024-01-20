#pragma once
#include <GMLIB/Files/JsonConfig.h>
#include <GMLIB/Files/JsonLanguage.h>

inline std::string defaultConfig = R"({
    "AutoCleanCount": {
        "Enabled": true,
        "TriggerCount": 900
    },
    "AutoCleanTPS": {
        "Enabled": false,
        "TriggerTPS": 15
    },
    "Basic": {
        "Command": "cleaner",
        "ConsoleLog": true,
        "Notice1": 20,
        "Notice2": 5,
        "SendBroadcast": true,
        "SendToast": false
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

inline std::string defaultLanguage = R"({
    "cleaner.command.cleaner": "Cleaner管理员命令。",
    "cleaner.command.despawnSuccess": "已成功清除了 %1$s 个实体",
    "cleaner.command.error.noTarget": "没有与选择器匹配的目标",
    "cleaner.command.error.playerOnly": "该命令只能由玩家执行！",
    "cleaner.command.tps.output": "当前服务器实时TPS %1$s §r，平均TPS %2$s",
    "cleaner.command.mspt.output": "当前服务器实MSPT %1$s",
    "cleaner.command.voteclean": "发起实体清理投票。",
    "cleaner.command.despawntime" : "已成功将物品消失时间设置为 %1$s 游戏刻",
    "cleaner.output.count1": "系统将在 %1$s 秒后自动清理服务器实体！",
    "cleaner.output.count2": "请注意！系统将在 %1$s 秒后自动清理服务器实体！",
    "cleaner.output.finish": "清理完成！本次总共清理了 %1$s 个实体。",
    "cleaner.output.opClean": "管理员启用了服务器实体清理。",
    "cleaner.output.reload": "已重载Cleaner插件，部分配置可能需要重启服务器才能生效。",
    "cleaner.output.triggerAutoCleanCount": "检测到服务器实体过多！！当前服务器存在 %1$s 个可清理实体，已启动自动清理程序。",
    "cleaner.output.triggerAutoCleanTps": "当前服务器TPS过低！！服务器平均TPS %1$s\n系统已启动清理程序。",
    "cleaner.vote.cooldown": "投票清理冷却...",
    "cleaner.vote.cancel": "投票已取消！",
    "cleaner.vote.confirmNo": "我再想想",
    "cleaner.vote.confirmOk": "发起投票",
    "cleaner.vote.confirmTubtitle": "你是否要发起投票清理？",
    "cleaner.vote.accept": "你已同意实体清理。",
    "cleaner.vote.deny": "你已拒绝实体清理。",
    "cleaner.vote.no": "拒绝",
    "cleaner.vote.ok": "同意",
    "cleaner.vote.Subtitle": "你是否同意现在清理服务器实体？",
    "cleaner.vote.Timeout": "投票已过期！",
    "cleaner.vote.succeed": "投票清理成功！",
    "cleaner.vote.title": "投票清理",
    "cleaner.vote.toastNotice": "通知"
})";