#pragma once

#include "Axiom/Core/Math.h"
#include "Axiom/Utilities/Memory/Ref.h"


namespace Axiom {

	/**
	 * Base class for all entities in the world.
	 * Every object that exists in the world with a position and can be
	 * tracked by the server (players, mobs, items, projectiles, etc.)
	 * inherits from Entity.
	 */
	class Entity : public virtual RefCounted {
	public:
		explicit Entity(const int32_t entityId)
			: m_EntityId(entityId) {}

		virtual ~Entity() = default;

		int32_t GetEntityId() const { return m_EntityId; }

		const Vector3& GetPosition() const { return m_Position; }
		void SetPosition(const Vector3& position) { m_Position = position; }

		const Vector2& GetRotation() const { return m_Rotation; }
		void SetRotation(const Vector2& rotation) { m_Rotation = rotation; }

		const Vector3& GetVelocity() const { return m_Velocity; }
		void SetVelocity(const Vector3& velocity) { m_Velocity = velocity; }

		bool IsOnGround() const { return m_OnGround; }
		void SetOnGround(const bool onGround) { m_OnGround = onGround; }

		int32_t GetChunkX() const { return m_Position.ChunkX(); }
		int32_t GetChunkZ() const { return m_Position.ChunkZ(); }

		virtual void Tick() {}

		bool IsRemoved() const { return m_Removed; }
		void Remove() { m_Removed = true; }

	protected:
		int32_t m_EntityId;
		Vector3 m_Position;
		Vector2 m_Rotation;   // x = yaw, y = pitch
		Vector3 m_Velocity;
		bool m_OnGround = false;
		bool m_Removed = false;
	};

}
