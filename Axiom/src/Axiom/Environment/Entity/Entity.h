#pragma once

#include "Axiom/Core/Math.h"
#include "Axiom/Core/Observable.h"
#include "Axiom/Core/Tickable.h"
#include "Axiom/Utilities/Memory/Ref.h"

#include <atomic>

namespace Axiom {

	/**
	 * Base class for all entities in the world.
	 * Every object that exists in the world with a
	 * position and can be tracked by the server (players,
	 * mobs, items, projectiles, etc.) inherits from Entity.
	 *
	 * Every Entity is assigned a globally unique int32
	 * ID at construction time via the static atomic
	 * counter `s_NextEntityId`. The counter starts at 1
	 * so ID 0 is reserved as a sentinel for "no entity".
	 * The counter is `memory_order_relaxed` — uniqueness
	 * is the only invariant we need from it, not a
	 * happens-before relationship.
	 */
	class Entity : public Tickable, public virtual RefCounted {
	public:
		Entity()
			: m_EntityId(s_NextEntityId.fetch_add(1, std::memory_order_relaxed)) {}

		virtual ~Entity() = default;

		int32_t GetEntityId() const { return m_EntityId; }

		Vector3 GetPosition() const { return m_Position.Get(); }
		void SetPosition(const Vector3& position) { m_Position = position; }

		Vector2 GetRotation() const { return m_Rotation.Get(); }
		void SetRotation(const Vector2& rotation) { m_Rotation = rotation; }

		Vector3 GetVelocity() const { return m_Velocity.Get(); }
		void SetVelocity(const Vector3& velocity) { m_Velocity = velocity; }

		bool IsOnGround() const { return m_OnGround; }
		void SetOnGround(const bool onGround) { m_OnGround = onGround; }

		int32_t GetChunkX() const { return m_Position.Get().ChunkX(); }
		int32_t GetChunkZ() const { return m_Position.Get().ChunkZ(); }

		virtual void OnTick(Timestep /* timestep */) {}

		bool IsRemoved() const { return m_Removed; }
		void Remove() { m_Removed = true; }

	protected:
		int32_t m_EntityId;
		Observable<Vector3> m_Position;
		Observable<Vector2> m_Rotation; // x = yaw, y = pitch
		Observable<Vector3> m_Velocity;
		bool m_OnGround = false;
		bool m_Removed = false;

	private:
		/**
		 * Global monotonic source of entity IDs.
		 * Starts at 1; ID 0 is reserved as a sentinel
		 * for "no entity". Relaxed memory order is
		 * sufficient — only uniqueness matters.
		 */
		static inline std::atomic<int32_t> s_NextEntityId{1};
	};

}
