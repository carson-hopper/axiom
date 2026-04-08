#include "SystemChatPacket.h"
namespace Axiom {

void SystemChatPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteChatComponent(mMessage);
    buffer.WriteBoolean(mOverlay);
}

template class SystemChatPacket<775>;

} // namespace Axiom