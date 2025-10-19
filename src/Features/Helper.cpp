#include "Global.h"
#include "gmlib/mc/world/Level.h"
namespace Helper {

void broadcastMessage(std::string_view msg) {
    gmlib::GMLevel::getInstance()->broadcast(tr("cleaner.info.prefix") + std::string(msg));
}

void broadcastToast(std::string_view msg) {
    gmlib::GMLevel::getInstance()->broadcastToast(tr("cleaner.info.prefix"), msg);
}

} // namespace Helper