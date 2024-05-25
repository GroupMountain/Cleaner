#include "Global.h"

namespace Helper {

void broadcastMessage(std::string_view msg) {
    GMLIB_Level::getInstance()->broadcast(tr("cleaner.info.prefix") + std::string(msg));
}

void broadcastToast(std::string_view msg) {
    GMLIB_Level::getInstance()->broadcastToast(tr("cleaner.info.prefix"), msg);
}

} // namespace Helper