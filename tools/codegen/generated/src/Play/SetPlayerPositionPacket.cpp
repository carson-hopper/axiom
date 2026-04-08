#include "SetPlayerPositionPacket.h"
namespace Axiom {

void SetPlayerPositionPacket::Deserialize(NetworkBuffer& buffer) {
    m_x = buffer.ReadDouble;
    m_y = buffer.ReadDouble;
    m_z = buffer.ReadDouble;
    m_on_ground = buffer.ReadBoolean;
}

void SetPlayerPositionPacket::Handle(Ref<Connection> connection, PacketContext& context) {
    // TODO: Implement packet handling
}

template class SetPlayerPositionPacket<775>;

} // namespace Axiom