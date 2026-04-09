#include "axpch.h"
#include "Axiom/Environment/Entity/ServerEntity.h"

#include <cmath>

namespace Axiom {

	ServerEntity::ServerEntity(Ref<Entity> entity)
		: m_Entity(std::move(entity)) {

		const auto& position = m_Entity->GetPosition();
		m_LastSentX = position.x;
		m_LastSentY = position.y;
		m_LastSentZ = position.z;

		const auto& rotation = m_Entity->GetRotation();
		m_LastSentYaw = rotation.x;
		m_LastSentPitch = rotation.y;
	}

	bool ServerEntity::HasPositionChanged() const {
		const auto& position = m_Entity->GetPosition();
		double diffX = position.x - m_LastSentX;
		double diffY = position.y - m_LastSentY;
		double diffZ = position.z - m_LastSentZ;
		return (diffX * diffX + diffY * diffY + diffZ * diffZ) >= (POSITION_THRESHOLD * POSITION_THRESHOLD);
	}

	bool ServerEntity::HasRotationChanged() const {
		const auto& rotation = m_Entity->GetRotation();
		float yawDiff = std::abs(rotation.x - m_LastSentYaw);
		float pitchDiff = std::abs(rotation.y - m_LastSentPitch);
		return yawDiff >= ROTATION_THRESHOLD || pitchDiff >= ROTATION_THRESHOLD;
	}

	bool ServerEntity::NeedsFullUpdate() const {
		const auto& position = m_Entity->GetPosition();
		double diffX = position.x - m_LastSentX;
		double diffY = position.y - m_LastSentY;
		double diffZ = position.z - m_LastSentZ;
		return std::sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ) >= TELEPORT_THRESHOLD;
	}

	void ServerEntity::AcknowledgeUpdate() {
		const auto& position = m_Entity->GetPosition();
		m_LastSentX = position.x;
		m_LastSentY = position.y;
		m_LastSentZ = position.z;

		const auto& rotation = m_Entity->GetRotation();
		m_LastSentYaw = rotation.x;
		m_LastSentPitch = rotation.y;

		m_TicksSinceLastUpdate = 0;
	}

	ServerEntity::DeltaUpdate ServerEntity::ComputeDelta() const {
		DeltaUpdate update;

		const auto& position = m_Entity->GetPosition();
		const auto& rotation = m_Entity->GetRotation();

		if (HasPositionChanged()) {
			update.hasPosition = true;
			update.deltaX = static_cast<int16_t>((position.x - m_LastSentX) * 4096.0);
			update.deltaY = static_cast<int16_t>((position.y - m_LastSentY) * 4096.0);
			update.deltaZ = static_cast<int16_t>((position.z - m_LastSentZ) * 4096.0);
		}

		if (HasRotationChanged()) {
			update.hasRotation = true;
			update.yaw = static_cast<uint8_t>(rotation.x * 256.0f / 360.0f);
			update.pitch = static_cast<uint8_t>(rotation.y * 256.0f / 360.0f);
		}

		return update;
	}

}
