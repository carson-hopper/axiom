#include "StatusPongPacket.h"
namespace Axiom {

void StatusPongPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteLong(m_payload);
}

template class StatusPongPacket<775>;

} // namespace Axiom