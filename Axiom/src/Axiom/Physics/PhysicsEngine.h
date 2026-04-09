#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Math.h"
#include "Axiom/Physics/AABB.h"

#include <vector>

namespace Axiom {

	class Entity;
	class Level;

	/**
	 * Handles entity movement, gravity,
	 * and collision resolution against
	 * the block world.
	 */
	class PhysicsEngine {
	public:
		static constexpr double Gravity = -0.08;
		static constexpr double DragFactor = 0.98;
		static constexpr double GroundFriction = 0.6;
		static constexpr double GroundCheckEpsilon = 0.001;

		PhysicsEngine() = default;
		~PhysicsEngine() = default;

		/**
		 * Move an entity by the given delta,
		 * resolving collisions against blocks
		 * and returning the final position.
		 */
		Vector3 CollideAndMove(Entity& entity, const Vector3& delta, Level& level);

		/**
		 * Apply gravitational acceleration
		 * and drag to an entity's velocity
		 * for one timestep.
		 */
		void ApplyGravity(Entity& entity, double timestep);

		/**
		 * Check whether the entity is standing
		 * on a solid block by probing a small
		 * distance below its bounding box.
		 */
		bool IsOnGround(const Entity& entity, Level& level);

	private:
		/**
		 * Collect AABBs of solid blocks that
		 * overlap the given expanded region.
		 */
		std::vector<AABB> GetBlockCollisions(const AABB& region, Level& level);
	};

}
