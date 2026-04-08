#include "SystemChatPacket.h"
namespace Axiom {

void SystemChatPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteChatComponent(m_message);
    buffer.WriteBoolean(m_overlay);
}

template class SystemChatPacket<775>;

} // namespace Axiom