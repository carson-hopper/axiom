#include "ClientTickEndPacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

PACKET_DECODE_BEGIN(ClientTickEndPacket)
PACKET_DECODE_END()

PACKET_HANDLE_BEGIN(ClientTickEndPacket)
    // No-op - client signals end of tick processing
PACKET_HANDLE_END()

PACKET_INSTANTIATE(ClientTickEndPacket, 775)

}
