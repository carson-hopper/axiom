#include "axpch.h"
#include "Axiom/Environment/Entity/Player.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Data/Nbt/NbtIo.h"
#include "Axiom/Event/EventBus.h"
#include "Axiom/Event/PlayerEvents.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Utilities/Permission.h"

namespace Axiom {

	Player::Player(Ref<Connection> connection, std::string name, const UUID& uuid)
		: LivingEntity(0)
		, m_Connection(std::move(connection))
		, m_Name(std::move(name))
		, m_Uuid(uuid) {

#ifdef AX_DEBUG
		GrantPermission("*");
#endif

		m_GameMode.Subscribe([this](const GameMode oldMode, const GameMode newMode) {
			PlayerGameModeChangedEvent event(this, oldMode, newMode);
			Application::Instance().Events().Publish(event);
		});
		m_FoodLevel.Subscribe([this](const int oldFood, const int newFood) {
			PlayerFoodChangedEvent event(this, oldFood, newFood);
			Application::Instance().Events().Publish(event);
		});
		m_ExperienceLevel.Subscribe([this](const int oldLevel, const int newLevel) {
			PlayerExperienceChangedEvent event(this, oldLevel, newLevel);
			Application::Instance().Events().Publish(event);
		});
		m_SelectedSlot.Subscribe([this](const int oldSlot, const int newSlot) {
			PlayerHotbarChangedEvent event(this, oldSlot, newSlot);
			Application::Instance().Events().Publish(event);
		});
	}

	void Player::SendMessage(const ChatText& message) {
		if (!m_Connection || !m_Connection->IsConnected() || !message) {
			return;
		}

		NetworkBuffer payload;
		NbtIo::WriteNetwork(message.Component()->ToNbt(), payload);
		payload.WriteBoolean(false); // Is overlay (action bar)

		m_Connection->SendRawPacket(Clientbound::Play::SystemChat, payload);
	}

	void Player::Kick(const ChatText& reason) const {
		if (!m_Connection || !m_Connection->IsConnected()) {
			return;
		}

		const auto component = reason
			? reason.Component()
			: ChatComponent::Create().Text("Disconnected").Build();

		NetworkBuffer payload;
		NbtIo::WriteNetwork(component->ToNbt(), payload);
		m_Connection->SendRawPacket(Clientbound::Play::Disconnect, payload);
		m_Connection->Disconnect(component->GetPlainText());
	}

	bool Player::HasPermission(const std::string& permission) const {
		for (const auto& owned : m_GrantedPermissions) {
			if (MatchesPermissionPattern(owned, permission)) {
				return true;
			}
		}
		return false;
	}

	void Player::OnDeath() {
		AX_CORE_INFO("Player {} died", m_Name);
	}

}
