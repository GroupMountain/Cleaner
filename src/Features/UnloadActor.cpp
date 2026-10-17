#include "Cleaner.h"
#include "Global.h"
#include "Mod.h"

#include "ll/api/base/Containers.h"
#include "ll/api/service/Bedrock.h"
#include "mc/common/Globals.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/storage/DBStorage.h"
#include "mc/world/level/storage/db_helpers/Category.h"
#include "mc/deps/nbt/CompoundTag.h"
#include "mc/deps/nbt/ListTag.h"
#include "mc/deps/nbt/StringTag.h"
#include <gmlib/mc/world/Level.h>
#include <gmlib/mc/world/Scoreboard.h>
#include <gmlib/mc/world/actor/UnloadedActor.h>


#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

OfflineEntityData OfflineEntity;

static bool UnloadActorToBrief(
    std::string_view     data,
    int64_t              storageKey,
    std::string&         outTypeName,
    UnloadedEntityBrief& outBrief
) {
    auto nbtOpt = CompoundTag::fromBinaryNbt(data);
    if (!nbtOpt || !nbtOpt->contains("UniqueID")) return false;
    auto& nbt = *nbtOpt;

    outBrief            = UnloadedEntityBrief{};
    outBrief.storageKey = storageKey;
    outBrief.id         = ActorUniqueID(nbt["UniqueID"]);

    if (nbt.contains("identifier")) {
        outTypeName = static_cast<std::string>(nbt["identifier"].get<StringTag>());
    } else {
        outTypeName = "unknown";
    }

    auto getByte = [&](std::string_view key) -> char {
        return nbt.contains(key) ? nbt.at(key).get<ByteTag>().data : 0;
    };
    outBrief.isTamed           = getByte("IsTamed") != 0;
    outBrief.isTrusting        = getByte("IsTrusting") != 0;
    outBrief.customNameVisible = getByte("CustomNameVisible") != 0;

    if (nbt.contains("Tags")) {
        auto& tagsList = nbt.at("Tags").get<ListTag>();
        for (auto& t : tagsList) {
            auto d = t->as<StringTag>().data();
            if (d != nullptr) outBrief.tags.emplace_back(d);
        }
    }

    if (outTypeName == "minecraft:item") {
        if (nbt.contains("Age")) {
            outBrief.hasItemAge = true;
            outBrief.itemAge    = nbt.at("Age").get<ShortTag>().data;
        }
        if (nbt.contains("Item")) {
            auto& itemNbt = nbt.at("Item").get<CompoundTag>();
            if (itemNbt.contains("Name")) {
                outBrief.hasItemName = true;
                outBrief.itemName    = itemNbt.at("Name").get<StringTag>().data();
            }
        }
    }

    if (outTypeName == "minecraft:arrow" && nbt.contains("auxValue")) {
        auto& auxTag = nbt.at("auxValue");
        if (auto& aux = auxTag.get<ByteTag>()) {
            outBrief.arrowAux = aux.data;
        }
    }

    return true;
}

//safety remove
static void removeUnloadActor(ActorUniqueID id, int64_t storageKey) {
    gmlib::GMScoreboard::getInstance()->resetScore(id);
    if (auto actor = ll::service::bedrock::getLevel()->fetchEntity(id, false)) {
        actor->remove();
        return;
    }
    auto keyStr = "actorprefix" + std::string(reinterpret_cast<const char*>(&storageKey), 8);
    gmlib::GMLevel::getInstance()->getDBStorage().deleteData(keyStr, DBHelpers::Category::Actor);
}

// ---------------------------------------------------------------------------
// 重建离线实体索引（轻量 Brief，不缓存 NBT）
// ---------------------------------------------------------------------------
void updateUnloadEntityMap(std::vector<std::string> CleanList, bool skipCheck) {
    // 防重入：多个调用同时进来只会有第一个执行
    if (OfflineEntity.isUpdating.exchange(true)) return;

    if (!(OfflineEntity.requireUpdate || skipCheck)) {
        OfflineEntity.isUpdating.store(false);
        return;
    }

    std::thread([CleanList = std::move(CleanList)]() mutable {
        try {
            BriefMap newMap;
            auto&    db = gmlib::GMLevel::getInstance()->getDBStorage();

            db.forEachKeyWithPrefix(
                "actorprefix",
                DBHelpers::Category::Actor,
                [&](std::string_view keyLeft, std::string_view data) {
                    if (keyLeft.size() < 8) return;
                    int64_t storageKey;
                    std::memcpy(&storageKey, keyLeft.data(), 8);

                    std::string         typeName;
                    UnloadedEntityBrief brief;
                    if (!UnloadActorToBrief(data, storageKey, typeName, brief)) {
                        db.deleteData(keyLeft.data(), DBHelpers::Category::Actor);
                        return;
                    }

                    if (typeName.empty() || typeName == "minecraft:player") return;

                    if (std::ranges::find(CleanList, typeName) != CleanList.end()) {
                        removeUnloadActor(brief.id, brief.storageKey);
                        return;
                    }

                    newMap[typeName].emplace_back(std::move(brief));
                }
            );

            {
                std::lock_guard<std::mutex> lock(OfflineEntity.offlineEntitiesMutex);
                for (auto& playerID : OfflineEntity.waitedPlayers) {
                    if (auto pl = ll::service::getLevel()->getPlayer(playerID)) {
                        pl->sendMessage(tr("cleaner.unload.updated.msg"));
                    }
                }
                OfflineEntity.offlineEntities.swap(newMap);
            }
            OfflineEntity.lastUpdate = std::chrono::system_clock::now();
        } catch (...) {
        }
        OfflineEntity.isUpdating.store(false);
    }).detach();
}