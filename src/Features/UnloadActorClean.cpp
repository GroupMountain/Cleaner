#include "Cleaner.h"
#include "Global.h"
#include "gmlib/mc/world/actor/UnloadedActor.h"

namespace UnloadActorClean {
void cleanUnloadActor() {
    auto& config = Cleaner::Entry::getInstance().getConfig();
    gmlib::UnloadedActor::foreachUnloadedActor(
        [config](gmlib::UnloadedActor& actor) -> bool {
            for (auto& actorname : config.UnloadActorClean.CleanList) {
                if (actor.getTypeName() == actorname) {
                    actor.remove();
                }
            }
            return true;
        }
    );
}
} // namespace UnloadActorClean