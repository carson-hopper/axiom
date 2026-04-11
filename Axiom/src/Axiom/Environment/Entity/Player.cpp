#include "axpch.h"
#include "Axiom/Environment/Entity/Player.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Data/Nbt/NbtIo.h"
#include "Axiom/Event/EventBus.h"
#include "Axiom/Event/PlayerEvents.h"
#include "Axiom/Network/Packet/Play/Clientbound/GameEventPacket.h"
#include "Axiom/Network/Packet/Play/Clientbound/PlayerPositionPacket.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Utilities/Permission.h"

namespace Axiom {

	Player::Player(Ref<Connection> connection, std::string name, const UUID& uuid)
		: m_Connection(std::move(connection))
		, m_Name(std::move(name))
		, m_Uuid(uuid) {

#ifdef AX_DEBUG
		GrantPermission("*");
#endif

		m_Position.Subscribe([this](const Vector3& oldPosition, const Vector3& newPosition) {
			PlayerPositionChangedEvent event(this, oldPosition, newPosition);
			Application::Instance().Events().Publish(event);
		});
		m_GameMode.Subscribe([this](const GameMode oldMode, const GameMode newMode) {
			PlayerGameModeChangedEvent event(this, oldMode, newMode);
			Application::Instance().Events().Publish(event);

			if (!m_Connection || !m_Connection->IsConnected()
				|| m_Connection->State() != ConnectionState::Play) {
				return;
			}

			Play::Clientbound::GameEventPacket packet{
				Play::Clientbound::GameEventType::ChangeGameMode,
				static_cast<float>(static_cast<int>(newMode)),
			};
			m_Connection->SendRawPacket(packet);
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
		m_OpLevel.Subscribe([this](const OpLevel oldLevel, const OpLevel newLevel) {
			PlayerOperatorLevelChangeEvent event(this, oldLevel, newLevel);
			Application::Instance().Events().Publish(event);
		});
	}

	bool Player::IsValidPosition(const Vector3& position) {
		if (!std::isfinite(position.x)
			|| !std::isfinite(position.y)
			|| !std::isfinite(position.z)) {
			return false;
		}

		constexpr double WorldBorderLimit = 30'000'000.0;
		if (std::abs(position.x) > WorldBorderLimit
			|| std::abs(position.z) > WorldBorderLimit) {
			return false;
		}

		constexpr double MinY = -512.0;
		constexpr double MaxY = 1024.0;
		if (position.y < MinY || position.y > MaxY) {
			return false;
		}

		return true;
	}

	bool Player::IsValidMoveTarget(const Vector3& position) const {
		if (!IsValidPosition(position)) {
			return false;
		}

		constexpr double MaxDeltaSquared = 100.0 * 100.0;
		if (position.DistanceSquaredTo(GetPosition()) > MaxDeltaSquared) {
			return false;
		}

		return true;
	}

	void Player::Teleport(const Vector3& position) {
		if (!IsValidPosition(position)) {
			AX_CORE_WARN(
				"Refusing to teleport {}: target ({:.2f}, {:.2f}, {:.2f}) "
				"failed positional sanity checks",
				m_Name, position.x, position.y, position.z);
			return;
		}

		if (m_PendingTeleportId.load(std::memory_order_acquire) == 0) {
			m_LastGoodPosition = GetPosition();
		}

		SetPosition(position);

		if (!m_Connection || !m_Connection->IsConnected()
			|| m_Connection->State() != ConnectionState::Play) {
			return;
		}

		const int32_t teleportId =
			m_NextTeleportId.fetch_add(1, std::memory_order_relaxed);
		m_PendingTeleportId.store(teleportId, std::memory_order_release);

		Play::Clientbound::PlayerPositionPacket packet{
			teleportId, position, Vector3{0.0, 0.0, 0.0}, m_Rotation.Get(), 0
		};
		m_Connection->SendRawPacket(packet);
	}

	void Player::ConfirmTeleport(const int32_t teleportId) {
		int32_t expected = teleportId;
		if (m_PendingTeleportId.compare_exchange_strong(expected, 0,
				std::memory_order_acq_rel, std::memory_order_relaxed)) {
			m_LastGoodPosition = GetPosition();
			return;
		}

		AX_CORE_WARN(
			"{} sent mismatched teleport ack (got {}, pending {}); "
			"reverting to last good",
			m_Name, teleportId, expected);
		SetPosition(m_LastGoodPosition);
		m_PendingTeleportId.store(0, std::memory_order_release);
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
