#include "LoginSuccessPacket.h"
namespace Axiom {

void LoginSuccessPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteUUID(m_uuid);
    buffer.WriteString(m_username);
}

template class LoginSuccessPacket<775>;

} // namespace Axiom