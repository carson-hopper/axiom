#include "KeepAliveResponsePacket.h"
namespace Axiom {

void KeepAliveResponsePacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteLong(mId);
}

template class KeepAliveResponsePacket<775>;

} // namespace Axiom