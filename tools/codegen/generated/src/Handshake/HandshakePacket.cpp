#include "HandshakePacket.h"
namespace Axiom {

void HandshakePacket::Deserialize(NetworkBuffer& buffer) {
    m_protocol_version = buffer.ReadVarInt;
    m_server_address = buffer.ReadString;
    m_server_port = buffer.ReadUnsignedShort;
    m_next_state = buffer.ReadVarInt;
}

void HandshakePacket::Handle(Ref<Connection> connection, PacketContext& context) {
    // TODO: Implement packet handling
}

template class HandshakePacket<775>;

} // namespace Axiom