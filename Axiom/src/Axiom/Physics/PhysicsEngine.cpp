#include "PhysicsEngine.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Environment/Entity/Entity.h"
#include "Axiom/Environment/Level/Level.h"

#include <cmath>

namespace Axiom {

	Vector3 PhysicsEngine::CollideAndMove(Entity& entity, const Vector3& delta, Level& level) {
		const Vector3 position = entity.GetPosition();

		// Entity bounding box at current position
		// Default hitbox: 0.6 wide, 1.8 tall (player-sized)
		const double halfWidth = 0.3;
		const double height = 1.8;
		AABB entityBox = AABB::FromCenterAndSize(position, halfWidth, height / 2.0);

		// Expand the search region to cover the full movement
		const AABB searchRegion = entityBox.Expand(1.0).Move(delta);
		const std::vector<AABB> collisions = GetBlockCollisions(searchRegion, level);

		double moveX = delta.x;
		double moveY = delta.y;
		double moveZ = delta.z;

		// Resolve Y axis first (gravity), then X and Z
		for (const auto& blockBox : collisions) {
			moveY = blockBox.ClipYCollide(entityBox, moveY);
		}
		entityBox = entityBox.Move(0.0, moveY, 0.0);

		for (const auto& blockBox : collisions) {
			moveX = blockBox.ClipXCollide(entityBox, moveX);
		}
		entityBox = entityBox.Move(moveX, 0.0, 0.0);

		for (const auto& blockBox : collisions) {
			moveZ = blockBox.ClipZCollide(entityBox, moveZ);
		}

		const Vector3 resolvedPosition = {
			position.x + moveX,
			position.y + moveY,
			position.z + moveZ
		};

		// Detect collisions that zeroed out movement
		Vector3 velocity = entity.GetVelocity();
		if (std::abs(moveX) < 1e-9 && std::abs(delta.x) > 1e-9) {
			velocity.x = 0.0;
		}
		if (std::abs(moveY) < 1e-9 && std::abs(delta.y) > 1e-9) {
			velocity.y = 0.0;
		}
		if (std::abs(moveZ) < 1e-9 && std::abs(delta.z) > 1e-9) {
			velocity.z = 0.0;
		}

		entity.SetPosition(resolvedPosition);
		entity.SetVelocity(velocity);
		entity.SetOnGround(moveY != delta.y && delta.y < 0.0);

		return resolvedPosition;
	}

	void PhysicsEngine::ApplyGravity(Entity& entity, double timestep) {
		Vector3 velocity = entity.GetVelocity();

		// Apply gravity
		velocity.y = velocity.y + Gravity * timestep;

		// Apply drag
		velocity.x = velocity.x * DragFactor;
		velocity.y = velocity.y * DragFactor;
		velocity.z = velocity.z * DragFactor;

		// Apply ground friction
		if (entity.IsOnGround()) {
			velocity.x = velocity.x * GroundFriction;
			velocity.z = velocity.z * GroundFriction;
		}

		entity.SetVelocity(velocity);
	}

	bool PhysicsEngine::IsOnGround(const Entity& entity, Level& level) {
		const Vector3 position = entity.GetPosition();
		const double halfWidth = 0.3;
		const double height = 1.8;

		// Check a thin box slightly below the entity's feet
		const AABB feetProbe(
			position.x - halfWidth,
			position.y - GroundCheckEpsilon,
			position.z - halfWidth,
			position.x + halfWidth,
			position.y,
			position.z + halfWidth
		);

		(void)height;

		const std::vector<AABB> collisions = GetBlockCollisions(feetProbe, level);
		return !collisions.empty();
	}

	std::vector<AABB> PhysicsEngine::GetBlockCollisions(const AABB& region, Level& level) {
		std::vector<AABB> collisions;

		const int minBlockX = static_cast<int>(std::floor(region.MinX()));
		const int minBlockY = static_cast<int>(std::floor(region.MinY()));
		const int minBlockZ = static_cast<int>(std::floor(region.MinZ()));
		const int maxBlockX = static_cast<int>(std::floor(region.MaxX()));
		const int maxBlockY = static_cast<int>(std::floor(region.MaxY()));
		const int maxBlockZ = static_cast<int>(std::floor(region.MaxZ()));

		for (int blockX = minBlockX; blockX <= maxBlockX; ++blockX) {
			for (int blockY = minBlockY; blockY <= maxBlockY; ++blockY) {
				for (int blockZ = minBlockZ; blockZ <= maxBlockZ; ++blockZ) {
					int32_t stateId = level.GetBlockState(blockX, blockY, blockZ);
					if (stateId != 0) {
						// Non-air block: treat as full cube
						collisions.push_back(AABB::BlockAABB(blockX, blockY, blockZ));
					}
				}
			}
		}

		return collisions;
	}

}
