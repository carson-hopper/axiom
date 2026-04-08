#include "StatusResponsePacket.h"
namespace Axiom {

void StatusResponsePacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteString(mJsonResponse);
}

template class StatusResponsePacket<775>;

} // namespace Axiom