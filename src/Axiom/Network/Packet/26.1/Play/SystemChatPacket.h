#pragma once

/**
 * @file SystemChatPacket.h
 * @brief System chat message packet (clientbound).
 *
 * Sent by the server to display system messages to players.
 * Used for commands, server announcements, death messages, etc.
 *
 * Packet ID: 0x79 (Clientbound::Play::SystemChat)
 */

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

namespace Axiom {

/**
 * Clientbound packet for system chat messages.
 *
 * Unlike PlayerChatPacket, this doesn't include sender information
 * and is used for server-generated messages.
 */
template<int32_t Version = PROTOCOL_VERSION>
class SystemChatPacket {
public:
	static constexpr int32_t PacketId = Clientbound::Play::SystemChat;
	static constexpr auto PacketState = ConnectionState::Play;

	/**
	 * Set the chat component (message content).
	 */
	void SetMessage(const Ref<ChatComponent>& message) {
		m_Message = message;
	}

	/**
	 * Set whether the message should appear on the action bar instead of chat.
	 */
	void SetActionBar(bool actionBar) {
		m_ActionBar = actionBar;
	}

	/**
	 * Encode the packet to a network buffer.
	 */
	void Encode(NetworkBuffer& buffer) const;

private:
	Ref<ChatComponent> m_Message;
	bool m_ActionBar = false;
};

}
