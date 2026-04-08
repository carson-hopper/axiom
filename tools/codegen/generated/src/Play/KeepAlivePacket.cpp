#include "KeepAlivePacket.h"
namespace Axiom {

void KeepAlivePacket::Deserialize(NetworkBuffer& buffer) {
    mId = buffer.ReadLong;
}

void KeepAlivePacket::Handle(Ref<Connection> connection, PacketContext& context) {
    // TODO: Implement packet handling
}

template class KeepAlivePacket<775>;

} // namespace Axiom