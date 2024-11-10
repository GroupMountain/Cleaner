# Cleaner
服务器实体清理插件

## 配置文件
```jsonc
{
    "version": 2, // 配置文件版本(勿动)
    "language": "zh_CN", // 语言(目前支持zh_CN和en_US)
    "Basic": { // 基础配置
        "Command": "cleaner", // 清理命令
        "Notice1": 20, // 提示1发送时间(剩余时间) (单位:秒)
        "Notice2": 5, // 提示2发送时间(剩余时间) (单位:秒)
        "ConsoleLog": true, // 是否在控制台输出清理日志
        "SendBroadcast": true, // 是否发送广播
        "SendToast": true // 是否发送Toast(屏幕上方)
    },
    "IgnoreTags": [ // 忽略的实体标签
        "ignore",
        "不清理"
    ],
    "AutoCleanCount": { // 自动清理数量配置
        "Enabled": true, // 是否启用
        "TriggerCount": 900 // 触发清理最低数量
    },
    "AutoCleanTPS": { // 自动清理TPS配置
        "Enabled": true, // 是否启用
        "TriggerTPS": 15 // 触发清理最低TPS
    },
    "ItemDespawn": { // 掉落物自然刷新配置
        "Enabled": true, // 是否启用
        "DespawnTime": 6000, // 刷新时间(单位:tick)
        "WhiteList": [ // 白名单
            "minecraft:elytra"
        ]
    },
    "CleanInanimate": { // 非生物实体清理配置
        "Enabled": true, // 是否启用
        "Blacklist": [ // 黑名单
            "minecraft:xp_orb",
            "minecraft:arrow",
            "minecraft:fireball",
            "minecraft:small_fireball",
            "minecraft:wither_skull",
            "minecraft:wither_skull_dangerous",
            "minecraft:dragon_fireball"
        ]
    },
    "CleanItem": { // 掉落物清理配置
        "Enabled": true, // 是否启用
        "ExistTicks": 0, // 安全时间(掉落物存在时间小于这个时间则不会清理) 单位:tick
        "Whitelist": [ // 白名单
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
    "CleanMobs": { // 生物实体清理配置
        "Enabled": true, // 是否启用
        "CleanMonstors": true, // 是否清理怪物
        "CleanPeacefulMobs": false, // 是否清理和平生物
        "EnableAutoExclude": true, // 无用
        "BlackList": [ // 黑名单
            "minecraft:guardian",
            "minecraft:zombie_pigman"
        ],
        "Whitelist": [ // 白名单
            "minecraft:ender_dragon",
            "minecraft:shulker",
            "minecraft:wither",
            "minecraft:elder_guardian",
            "minecraft:piglin_brute",
            "minecraft:ender_pearl",
            "minecraft:phantom"
        ]
    },
    "ScheduleClean": { // 定时清理配置
        "Enabled": true, // 是否启用
        "CleanInterval": 3600 // 清理间隔 单位:秒
    },
    "VoteClean": { // 投票清理配置
        "Enabled": true, // 是否启用
        "VoteCleanCommand": "voteclean", // 投票清理指令
        "Cooldown": 120, // 投票冷却时间 单位:秒
        "CheckDelay": 30, // 投票时长 单位:秒
        "Percentage": 50 // 投票百分比(大于此值则清理)
    }
}
```

# 开源许可
## 源代码可用性
- 您可以自由地获取、使用和修改本插件的源代码，无论是个人使用还是商业目的。
## 修改发布
- 如果您对本插件进行了修改或衍生创作，并打算分发、发布该修改或衍生作品，您必须开源并且以GPL3.0协议下相同的许可证条件进行分发。
## 版权声明
- 在您分发或发布基于GPL3.0协议的软件时（包括但不限于本插件以及本插件的衍生作品），您必须保留原始版权声明、许可证说明和免责声明。
## 引用链接
- 如果您在一个作品中使用了本插件或者本插件的源码，您需要提供一个明确的引用链接，指向软件的许可证和源代码。
## 对整体的影响
- 如果您将基于本插件与其他插件结合使用，或整合成一个单一的插件，那么整个插件都需要遵守GPL3.0协议进行开源。
