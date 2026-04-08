#include "StatusPingPacket.h"
namespace Axiom {

void StatusPingPacket::Deserialize(NetworkBuffer& buffer) {
    mPayload = buffer.ReadLong;
}

void StatusPingPacket::Handle(Ref<Connection> connection, PacketContext& context) {
    // TODO: Implement packet handling
}

template class StatusPingPacket<775>;

} // namespace Axiom