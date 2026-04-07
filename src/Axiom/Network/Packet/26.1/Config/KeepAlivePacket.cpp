#include "KeepAlivePacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

PACKET_DECODE_BEGIN(ConfigKeepAlivePacket)
    READ_INT64(m_KeepAliveId)
PACKET_DECODE_END()

PACKET_HANDLE_BEGIN(ConfigKeepAlivePacket)
    AX_CORE_TRACE("Config keep-alive response from {}: {}", connection->RemoteAddress(), m_KeepAliveId);
PACKET_HANDLE_END()

PACKET_INSTANTIATE(ConfigKeepAlivePacket, 775)

}
