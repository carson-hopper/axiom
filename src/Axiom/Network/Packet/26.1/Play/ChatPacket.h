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
	/**
	 * Get the chat message content.
	 */
	const std::string& GetMessage() const { return m_Message; }

	/**
	 * Get the timestamp when the message was sent.
	 */
	int64_t GetTimestamp() const { return m_Timestamp; }

	/**
	 * Get the salt for message verification.
	 */
	int64_t GetSalt() const { return m_Salt; }

	/**
	 * Check if the message has a signature (signed chat).
	 */
	bool HasSignature() const { return !m_Signature.empty(); }

	/**
	 * Get the message signature (if signed).
	 */
	const std::vector<uint8_t>& GetSignature() const { return m_Signature; }

private:
	std::string m_Message;
	int64_t m_Timestamp = 0;
	int64_t m_Salt = 0;
	std::vector<uint8_t> m_Signature;
PACKET_DECL_END()

}
