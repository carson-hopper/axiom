#include "PlayKeepAlivePacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

PACKET_DECODE_BEGIN(PlayKeepAlivePacket)
	READ_LONG(m_KeepAliveId);
PACKET_DECODE_END()

PACKET_HANDLE_BEGIN(PlayKeepAlivePacket)
	context.KeepAlive().OnKeepAliveResponse(connection->Id(), m_KeepAliveId);
PACKET_HANDLE_END()

PACKET_INSTANTIATE(PlayKeepAlivePacket, 775)

}
