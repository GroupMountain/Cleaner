#include "Cleaner.h"

namespace Cleaner {

bool isMatch(std::string& A, std::string& B) {
    // 如果B是正则表达式
    if (B.find_first_of(".*+?()[]{}|^$") != std::string::npos) {
        try {
            std::regex regex(B);
            return std::regex_match(A, regex);
        } catch (...) {
            return false;
        }
    }
    return (A == B);
}

bool shouldIgnore(GMLIB_Actor* ac) {
    if (ac->isMob() || ac->isItemActor()) {
        if (ac->isTame() || ac->isTrusting() || ac->getNameTag() != "" || ac->hasTag("cleaner:ignore")) {
            return true;
        }
    }
    return false;
}

bool ShouldClean(Actor* actor) {
    // Players
    auto en = (GMLIB_Actor*)actor;
    if (en->isPlayer() || shouldIgnore(en)) {
        return false;
    }
    auto type = en->getTypeName();
    for (auto& tag : ConfigFile::mIgnoreTags) {
        if (en->hasTag(tag)) {
            return false;
        }
    }
    // Items
    if (en->isItemActor()) {
        if (Config->getValue<bool>({"CleanItem", "Enabled"}, false)) {
            auto itac = (ItemActor*)en;
            if (itac->age() <= Config->getValue<int>({"CleanItem", "ExistTicks"}, 0)) {
                return false;
            }
            auto itemType  = itac->item().getTypeName();
            auto whitelist = Config->getValue<std::vector<std::string>>({"CleanItem", "Whitelist"}, {});
            for (auto& key : whitelist) {
                if (isMatch(itemType, key)) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    // Mobs
    else if (en->isMob()) {
        if (Config->getValue<bool>({"CleanMobs", "Enabled"}, false)) {
            auto blacklist = Config->getValue<std::vector<std::string>>({"CleanMobs", "Blacklist"}, {});
            for (auto& key : blacklist) {
                if (isMatch(type, key)) {
                    return true;
                }
            }
            auto whitelist = Config->getValue<std::vector<std::string>>({"CleanMobs", "Whitelist"}, {});
            for (auto& key : whitelist) {
                if (isMatch(type, key)) {
                    return false;
                }
            }
            if (Config->getValue<bool>({"CleanMobs", "CleanMonstors"}, false)
                && en->hasCategory(ActorCategory::Monster)) {
                return true;
            }
            if (Config->getValue<bool>({"CleanMobs", "CleanPeacefulMobs"}, false)) {
                return true;
            }
        }
        return false;
    }
    // Others
    else {
        if (Config->getValue<bool>({"CleanInanimate", "Enabled"}, false)) {
            auto blacklist = Config->getValue<std::vector<std::string>>({"CleanInanimate", "Blacklist"}, {});
            for (auto& key : blacklist) {
                if (isMatch(type, key)) {
                    return true;
                }
            }
        }
        return false;
    }
}

int ExecuteClean() {
    int  clean_count  = 0;
    auto all_entities = GMLIB_Level::getLevel()->getAllEntities();
    for (auto entity : all_entities) {
        if (ShouldClean(entity)) {
            entity->despawn();
            clean_count++;
        }
    }
    return clean_count;
}

int CountEntities() {
    int  clean_count  = 0;
    auto all_entities = GMLIB_Level::getLevel()->getAllEntities();
    for (auto entity : all_entities) {
        if (ShouldClean(entity)) {
            clean_count++;
        }
    }
    return clean_count;
}

} // namespace Cleaner
