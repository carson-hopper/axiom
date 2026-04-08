#include "KeepAlivePacket.h"
namespace Axiom {

void KeepAlivePacket::Deserialize(NetworkBuffer& buffer) {
    m_id = buffer.ReadLong;
}

void KeepAlivePacket::Handle(Ref<Connection> connection, PacketContext& context) {
    // TODO: Implement packet handling
}

template class KeepAlivePacket<775>;

} // namespace Axiom