#include "KeepAlivePacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

PACKET_DECODE_BEGIN(ConfigKeepAlivePacket)
	READ_LONG(m_Id);
PACKET_DECODE_END()

PACKET_HANDLE_BEGIN(ConfigKeepAlivePacket)
	AX_CORE_TRACE("Config keep-alive response from {}: {}", connection->RemoteAddress(), m_Id);
PACKET_HANDLE_END()

PACKET_INSTANTIATE(ConfigKeepAlivePacket, 775)

}
