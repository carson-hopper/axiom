#include "HandshakePacket.h"
namespace Axiom {

void HandshakePacket::Deserialize(NetworkBuffer& buffer) {
    mProtocolVersion = buffer.ReadVarInt;
    mServerAddress = buffer.ReadString;
    mServerPort = buffer.ReadUnsignedShort;
    mNextState = buffer.ReadVarInt;
}

void HandshakePacket::Handle(Ref<Connection> connection, PacketContext& context) {
    // TODO: Implement packet handling
}

template class HandshakePacket<775>;

} // namespace Axiom