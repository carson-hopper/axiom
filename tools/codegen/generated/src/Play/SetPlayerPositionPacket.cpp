#include "SetPlayerPositionPacket.h"
namespace Axiom {

void SetPlayerPositionPacket::Deserialize(NetworkBuffer& buffer) {
    mX = buffer.ReadDouble;
    mY = buffer.ReadDouble;
    mZ = buffer.ReadDouble;
    mOnGround = buffer.ReadBoolean;
}

void SetPlayerPositionPacket::Handle(Ref<Connection> connection, PacketContext& context) {
    // TODO: Implement packet handling
}

template class SetPlayerPositionPacket<775>;

} // namespace Axiom