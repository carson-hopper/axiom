#include "LoginSuccessPacket.h"
namespace Axiom {

void LoginSuccessPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteUUID(mUuid);
    buffer.WriteString(mUsername);
}

template class LoginSuccessPacket<775>;

} // namespace Axiom