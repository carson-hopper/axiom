#pragma once

/**
 * @file PlayerChatPacket.h
 * @brief Player chat message packet (clientbound).
 *
 * Sent by the server to broadcast a chat message from a player to other players.
 * This is used for regular player chat messages (not system messages).
 *
 * Packet ID: 0x41 (Clientbound::Play::PlayerChat)
 */

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"
#include "Axiom/Network/Packet/PacketMacros.h"
#include "Axiom/Network/Protocol.h"

#include <string>
#include <vector>

namespace Axiom {

/**
 * Clientbound packet for player chat messages.
 *
 * Unlike SystemChatPacket, this includes sender information and is used
 * for actual player chat messages that appear in the chat log.
 */
CLIENTBOUND_PACKET_DECL_BEGIN(PlayerChatPacket, Play, Clientbound::Play::PlayerChat)

	/**
	 * Set the chat component (message content).
	 */
	void SetMessage(const Ref<ChatComponent>& message) {
		m_Message = message;
	}

	/**
	 * Get the chat component (message content).
	 */
	const Ref<ChatComponent>& GetMessage() const {
		return m_Message;
	}

	/**
	 * Set the sender's UUID.
	 */
	void SetSenderUUID(const std::string& uuid) {
		m_SenderUUID = uuid;
	}

	/**
	 * Get the sender's UUID.
	 */
	const std::string& GetSenderUUID() const {
		return m_SenderUUID;
	}

	/**
	 * Set the sender's display name.
	 */
	void SetSenderName(const std::string& name) {
		m_SenderName = name;
	}

	/**
	 * Get the sender's display name.
	 */
	const std::string& GetSenderName() const {
		return m_SenderName;
	}

	/**
	 * Set the message timestamp.
	 */
	void SetTimestamp(const int64_t timestamp) {
		m_Timestamp = timestamp;
	}

	/**
	 * Get the message timestamp.
	 */
	int64_t GetTimestamp() const {
		return m_Timestamp;
	}

	/**
	 * Set the message signature (for signed chat).
	 */
	void SetSignature(const std::vector<uint8_t>& signature) {
		m_Signature = signature;
		m_Signed = !signature.empty();
	}

	/**
	 * Get the message signature.
	 */
	const std::vector<uint8_t>& GetSignature() const {
		return m_Signature;
	}

	/**
	 * Check if the message is signed.
	 */
	bool IsSigned() const {
		return m_Signed;
	}

private:
	Ref<ChatComponent> m_Message;
	std::string m_SenderUUID;
	std::string m_SenderName;
	int64_t m_Timestamp = 0;
	std::vector<uint8_t> m_Signature;
	bool m_Signed = false;
CLIENTBOUND_PACKET_DECL_END()

}
