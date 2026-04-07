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

#include "Axiom/Network/Packet/ServerboundPacket.h"
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
template<int32_t Version = PROTOCOL_VERSION>
class PlayerChatPacket {
public:
	static constexpr int32_t PacketId = Clientbound::Play::PlayerChat;
	static constexpr auto PacketState = ConnectionState::Play;

	/**
	 * Set the chat component (message content).
	 */
	void SetMessage(const Ref<ChatComponent>& message) {
		m_Message = message;
	}

	/**
	 * Set the sender's UUID.
	 */
	void SetSenderUUID(const std::string& uuid) {
		m_SenderUUID = uuid;
	}

	/**
	 * Set the sender's display name.
	 */
	void SetSenderName(const std::string& name) {
		m_SenderName = name;
	}

	/**
	 * Set the message timestamp.
	 */
	void SetTimestamp(int64_t timestamp) {
		m_Timestamp = timestamp;
	}

	/**
	 * Encode the packet to a network buffer.
	 */
	void Encode(NetworkBuffer& buffer) const;

private:
	Ref<ChatComponent> m_Message;
	std::string m_SenderUUID;
	std::string m_SenderName;
	int64_t m_Timestamp = 0;
	std::vector<uint8_t> m_Signature;
	bool m_Signed = false;
};

}
