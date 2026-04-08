#include "StatusResponsePacket.h"
namespace Axiom {

void StatusResponsePacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteString(m_json_response);
}

template class StatusResponsePacket<775>;

} // namespace Axiom