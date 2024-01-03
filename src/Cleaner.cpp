#include "Global.h"

namespace Cleaner {

bool ShouldClean(Actor* en) {
    if (en->isType(ActorType::Player)) {
        return false;
    }
    // 过滤
    // Todo
    return true;
}

int ExecuteClean() {
    int  clean_count  = 0;
    auto all_entities = ll::service::bedrock::getLevel()->getRuntimeActorList();
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
    auto all_entities = ll::service::bedrock::getLevel()->getRuntimeActorList();
    for (auto entity : all_entities) {
        if (ShouldClean(entity)) {
            clean_count++;
        }
    }
    return clean_count;
}

void CleanSchedule() {
    // Todo Schedule
    auto count = ExecuteClean();
    // Todo Notice & Console Log
}

} // namespace Cleaner