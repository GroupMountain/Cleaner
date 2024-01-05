#include "Global.h"

int item_despawn_time = 3000;

LL_AUTO_TYPED_INSTANCE_HOOK(
    ItemSpawnEventHook,
    ll::memory::HookPriority::Normal,
    Spawner,
    "?spawnItem@Spawner@@QEAAPEAVItemActor@@AEAVBlockSource@@AEBVItemStack@@PEAVActor@@AEBVVec3@@H@Z",
    ItemActor*,
    class BlockSource&     region,
    class ItemStack const& item,
    class Actor*           spawner,
    class Vec3 const&      pos,
    int                    throwTime
) {
    auto itac = origin(region, item, spawner, pos, throwTime);
    if (itac) {
        if (spawner) {
            if (spawner->isType(ActorType::Player)) {
                if (!spawner->isAlive()) {
                    // Death Drop
                    return itac;
                }
            }
        }
        itac->mLifeTime = item_despawn_time;
    }
    return itac;
}

LL_AUTO_TYPED_INSTANCE_HOOK(
    ItemTakeEventHook,
    ll::memory::HookPriority::Normal,
    Actor,
    "?pickUpItem@Actor@@QEAAXAEAVItemActor@@H@Z",
    void,
    class ItemActor& itemActor, int count
) {
    logger.warn("pickup item {}", this->getTypeName());
    return origin(itemActor,count);
}