#include "HandshakePacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	// ----- Packet Decoding ------------------------------------------

	PACKET_DECODE_BEGIN(HandshakePacket)
		READ_VARINT(m_ProtocolVersion);
		READ_STRING_MAX(m_ServerAddress, 255);
		READ_USHORT(m_ServerPort);
		READ_VARINT(m_NextState);
	PACKET_DECODE_END()

	// ----- Packet Handling ------------------------------------------

	PACKET_HANDLE_BEGIN(HandshakePacket)
		AX_CORE_TRACE("Handshake from {}: protocol={}, address={}:{}, nextState={}",
			connection->RemoteAddress(), m_ProtocolVersion, m_ServerAddress, m_ServerPort, m_NextState);

		connection->ProtocolVersion(m_ProtocolVersion);

		if (m_NextState == 1) {
			connection->State(ConnectionState::Status);
		} else if (m_NextState == 2) {
			connection->State(ConnectionState::Login);
		} else {
			connection->Disconnect("Invalid next state");
		}
	PACKET_HANDLE_END()

	// ----- Template Instantiation -----------------------------------

	PACKET_INSTANTIATE(HandshakePacket, 775)

}