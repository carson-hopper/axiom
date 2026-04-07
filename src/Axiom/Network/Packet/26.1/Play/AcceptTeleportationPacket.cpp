#include "AcceptTeleportationPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

PACKET_DECODE_BEGIN(AcceptTeleportationPacket)
    READ_VARINT(m_TeleportId)
PACKET_DECODE_END()

PACKET_HANDLE_BEGIN(AcceptTeleportationPacket)
    AX_CORE_TRACE("Teleport accepted from {}: id={}", connection->RemoteAddress(), m_TeleportId);
PACKET_HANDLE_END()

PACKET_INSTANTIATE(AcceptTeleportationPacket, 775)

}
