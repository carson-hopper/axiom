#include "StatusRequestPacket.h"
namespace Axiom {

void StatusRequestPacket::Deserialize(NetworkBuffer& buffer) {
}

void StatusRequestPacket::Handle(Ref<Connection> connection, PacketContext& context) {
    // TODO: Implement packet handling
}

template class StatusRequestPacket<775>;

} // namespace Axiom