#include "Global.h"

namespace Helper {

void broadcastMessage(std::string msg) {
    TextPacket::createRawMessage(tr("cleaner.info.prefix") + msg).sendToClients();
}

void broadcastToast(std::string msg) {
    auto pkt = ToastRequestPacket(tr("cleaner.info.prefix"), msg);
    pkt.sendToClients();
}

} // namespace Helper