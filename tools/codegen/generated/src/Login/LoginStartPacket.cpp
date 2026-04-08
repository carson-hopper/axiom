#include "LoginStartPacket.h"
namespace Axiom {

void LoginStartPacket::Deserialize(NetworkBuffer& buffer) {
    m_username = buffer.ReadString;
}

void LoginStartPacket::Handle(Ref<Connection> connection, PacketContext& context) {
    // TODO: Implement packet handling
}

template class LoginStartPacket<775>;

} // namespace Axiom