#include "StatusPongPacket.h"
namespace Axiom {

void StatusPongPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteLong(mPayload);
}

template class StatusPongPacket<775>;

} // namespace Axiom