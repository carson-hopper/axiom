#include "PingRequestPacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	// ----- Packet Decoding ------------------------------------------

	PACKET_DECODE_BEGIN(PingRequestPacket)
		READ_INT64(m_Payload);
	PACKET_DECODE_END()

	// ----- Packet Handling ------------------------------------------

	PACKET_HANDLE_BEGIN(PingRequestPacket)
		NetworkBuffer payload;
		payload.WriteLong(m_Payload);
		connection->SendRawPacket(Clientbound::Status::PongResponse, payload);
	PACKET_HANDLE_END()

	// ----- Template Instantiation -----------------------------------

	PACKET_INSTANTIATE(PingRequestPacket, 775)

}