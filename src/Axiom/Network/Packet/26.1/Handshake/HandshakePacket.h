#pragma once

/**
 * @file HandshakePacket.h
 * @brief Initial handshake packet sent by the client.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

#include <string>

namespace Axiom {

	PACKET_DECL_BEGIN(HandshakePacket, Handshake, 0x00)
		PACKET_FIELD_INT32(ProtocolVersion)
		PACKET_FIELD_STRING(ServerAddress)
		PACKET_FIELD_UINT16(ServerPort)
		PACKET_FIELD_INT32(NextState)
	PACKET_DECL_END()

}
