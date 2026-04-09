#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Entity/Entity.h"

#include <cstdint>

namespace Axiom {

	/**
	 * Server-side tracking wrapper that monitors
	 * per-entity changes and produces delta-
	 * encoded position/rotation updates
	 * for network sync.
	 */
	class ServerEntity {
	public:
		explicit ServerEntity(Ref<Entity> entity);

		/** Check if position changed enough to send a delta update. */
		bool HasPositionChanged() const;

		/** Check if rotation changed enough to send an update. */
		bool HasRotationChanged() const;

		/** Whether a full (teleport) update is needed instead of delta. */
		bool NeedsFullUpdate() const;

		/** Mark the current state as sent, resetting deltas. */
		void AcknowledgeUpdate();

		/** Delta-encoded position/rotation change. */
		struct DeltaUpdate {
			int16_t deltaX = 0;
			int16_t deltaY = 0;
			int16_t deltaZ = 0;
			bool hasPosition = false;
			bool hasRotation = false;
			uint8_t yaw = 0;
			uint8_t pitch = 0;
		};

		/** Get the delta-encoded position change. */
		DeltaUpdate ComputeDelta() const;

		/** Get the tracked entity. */
		Entity& TrackedEntity() { return *m_Entity; }
		const Entity& TrackedEntity() const { return *m_Entity; }

		/** Tracking distance (blocks). Entities beyond this won't send updates. */
		int TrackingRange() const { return m_TrackingRange; }
		void SetTrackingRange(int range) { m_TrackingRange = range; }

	private:
		Ref<Entity> m_Entity;

		// Last sent state for delta encoding
		double m_LastSentX = 0.0;
		double m_LastSentY = 0.0;
		double m_LastSentZ = 0.0;
		float m_LastSentYaw = 0.0f;
		float m_LastSentPitch = 0.0f;

		int m_TrackingRange = 64;
		int m_TicksSinceLastUpdate = 0;

		static constexpr double POSITION_THRESHOLD = 0.0625;  // 1/16 of a block
		static constexpr float ROTATION_THRESHOLD = 1.0f;     // degrees
		static constexpr double TELEPORT_THRESHOLD = 8.0;     // blocks -- force full update
	};

}
