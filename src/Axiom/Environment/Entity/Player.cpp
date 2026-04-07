#include "Player.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Protocol.h"

#include <nlohmann/json.hpp>

namespace Axiom {

	void Player::SendMessage(const std::string& message) {
		if (!m_Connection || !m_Connection->IsConnected()) return;

		nlohmann::json chatMessage;
		chatMessage["text"] = message;

		NetworkBuffer payload;
		payload.WriteString(chatMessage.dump());
		payload.WriteBoolean(false); // Is overlay (action bar)

		m_Connection->SendRawPacket(Clientbound::Play::SystemChat, payload);
	}

	void Player::Kick(const std::string& reason) const {
		if (!m_Connection || !m_Connection->IsConnected()) return;

		nlohmann::json disconnectMessage;
		disconnectMessage["text"] = reason.empty() ? "Disconnected" : reason;

		NetworkBuffer payload;
		payload.WriteString(disconnectMessage.dump());

		m_Connection->SendRawPacket(Clientbound::Play::Disconnect, payload);
		m_Connection->Disconnect(reason);
	}

	void Player::OnDeath() {
		AX_CORE_INFO("Player {} died", m_Name);
	}

}
