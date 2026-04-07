#pragma once

/**
 * @file ChatPacket.h
 * @brief Player chat message packet (serverbound).
 *
 * Sent by the client when the player sends a chat message.
 * Packet ID: 0x09 (Serverbound::Play::Chat)
 */

#include "Axiom/Network/Packet/PacketMacros.h"

#include <string>

namespace Axiom {

	PACKET_DECL_BEGIN(ChatPacket, Play, Serverbound::Play::Chat)
		PACKET_FIELD_STRING(Message)
		PACKET_FIELD_INT64(Timestamp)
		PACKET_FIELD_INT64(Salt)
		PACKET_FIELD(std::vector<uint8_t>, Signature, {})

		/**
		 * Check if the message has a signature (signed chat).
		 */
		bool HasSignature() const { return !m_Signature.empty(); }
	PACKET_DECL_END()

}
