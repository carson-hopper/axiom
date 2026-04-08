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

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"
#include "Axiom/Network/Packet/PacketMacros.h"
#include "Axiom/Network/Protocol.h"

namespace Axiom {

/**
 * Clientbound packet for system chat messages.
 *
 * Unlike PlayerChatPacket, this doesn't include sender information
 * and is used for server-generated messages.
 */
CLIENTBOUND_PACKET_DECL_BEGIN(SystemChatPacket, Play, Clientbound::Play::SystemChat)

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
	 * Set whether the message should appear on the action bar instead of chat.
	 */
	void SetActionBar(const bool actionBar) {
		m_ActionBar = actionBar;
	}

	/**
	 * Get whether the message should appear on the action bar.
	 */
	bool IsActionBar() const {
		return m_ActionBar;
	}

private:
	Ref<ChatComponent> m_Message;
	bool m_ActionBar = false;
CLIENTBOUND_PACKET_DECL_END()

}
