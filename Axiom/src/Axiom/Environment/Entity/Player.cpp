#include "axpch.h"
#include "Axiom/Environment/Entity/Player.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Chat/ChatComponent.h"

#include <nlohmann/json.hpp>

namespace Axiom {

	void Player::SendMessage(const Ref<ChatComponent>& message) {
		if (!m_Connection || !m_Connection->IsConnected()) return;

		NetworkBuffer payload;
		payload.WriteTextComponent(message->ToJson());
		payload.WriteBoolean(false); // Is overlay (action bar)

		m_Connection->SendRawPacket(Clientbound::Play::SystemChat, payload);
	}

	void Player::Kick(const std::string& reason) const {
		if (!m_Connection || !m_Connection->IsConnected()) return;

		nlohmann::json disconnectMessage;
		disconnectMessage["text"] = reason.empty() ? "Disconnected" : reason;

		NetworkBuffer payload;
		payload.WriteTextComponent(disconnectMessage.dump());

		m_Connection->SendRawPacket(Clientbound::Play::Disconnect, payload);
		m_Connection->Disconnect(reason);
	}

	void Player::OnDeath() {
		AX_CORE_INFO("Player {} died", m_Name);
	}

}
