#include "Axiom/Environment/Level/Generator/CaveGenerator.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace Axiom {

	void CaveGenerator::Generate(int64_t worldSeed, int chunkX, int chunkZ,
		std::vector<int32_t>& blocks) const {

		for (int offsetX = -ChunkRadius; offsetX <= ChunkRadius; offsetX++) {
			for (int offsetZ = -ChunkRadius; offsetZ <= ChunkRadius; offsetZ++) {
				int originChunkX = chunkX + offsetX;
				int originChunkZ = chunkZ + offsetZ;

				int64_t chunkSeed = worldSeed ^
					(static_cast<int64_t>(originChunkX) * 341873128712LL) ^
					(static_cast<int64_t>(originChunkZ) * 132897987541LL);

				GenerateChunk(chunkSeed, originChunkX, originChunkZ,
					chunkX, chunkZ, blocks);
			}
		}
	}

	void CaveGenerator::GenerateChunk(int64_t seed, int originChunkX, int originChunkZ,
		int targetChunkX, int targetChunkZ,
		std::vector<int32_t>& blocks) const {

		std::mt19937_64 random(static_cast<uint64_t>(seed));

		// Determine number of cave starts in this chunk (1 in 7 chance per attempt)
		int attemptCount = static_cast<int>(random() % 15) + 1;

		for (int attempt = 0; attempt < attemptCount; attempt++) {
			// Random starting position within chunk
			double startX = static_cast<double>(originChunkX * 16) +
				static_cast<double>(random() % 16);
			double startY = static_cast<double>(random() % 120) + 8.0;
			double startZ = static_cast<double>(originChunkZ * 16) +
				static_cast<double>(random() % 16);

			// 1 in 7 chance to actually spawn a cave system
			if (random() % 7 != 0) continue;

			int tunnelCount = 1;

			// Small chance for a larger cave system with multiple branches
			if (random() % 4 == 0) {
				// Large room at the start
				float roomRadius = 1.0f + static_cast<float>(random() % 60) / 10.0f;
				CarveTunnel(static_cast<int64_t>(random()), originChunkX, originChunkZ,
					targetChunkX, targetChunkZ,
					startX, startY, startZ,
					roomRadius, 0.0f, 0.0f,
					-1, 0, blocks);
				tunnelCount += static_cast<int>(random() % 4);
			}

			for (int tunnel = 0; tunnel < tunnelCount; tunnel++) {
				float yaw = static_cast<float>(random() % 628) / 100.0f;
				float pitch = (static_cast<float>(random() % 628) / 100.0f - 3.14f) * 0.25f;
				float radius = static_cast<float>(random() % 200) / 100.0f + 1.0f;

				// Occasional wider tunnels
				if (random() % 10 == 0) {
					radius *= 2.0f + static_cast<float>(random() % 100) / 100.0f;
				}

				int totalSteps = static_cast<int>(ChunkRadius * 16 - random() % (ChunkRadius * 4));

				CarveTunnel(static_cast<int64_t>(random()), originChunkX, originChunkZ,
					targetChunkX, targetChunkZ,
					startX, startY, startZ,
					radius, yaw, pitch,
					0, totalSteps, blocks);
			}
		}
	}

	void CaveGenerator::CarveTunnel(int64_t seed, int originChunkX, int originChunkZ,
		int targetChunkX, int targetChunkZ,
		double startX, double startY, double startZ,
		float radius, float yaw, float pitch,
		int startStep, int totalSteps,
		std::vector<int32_t>& blocks) const {

		std::mt19937_64 random(static_cast<uint64_t>(seed));
		(void)originChunkX;
		(void)originChunkZ;

		double centerX = static_cast<double>(targetChunkX * 16 + 8);
		double centerZ = static_cast<double>(targetChunkZ * 16 + 8);

		float yawDrift = 0.0f;
		float pitchDrift = 0.0f;

		if (totalSteps <= 0) {
			int range = ChunkRadius * 16 - 16;
			totalSteps = range - static_cast<int>(random() % (range / 4));
		}

		// Single large room (startStep == -1)
		bool isRoom = false;
		int splitPoint = totalSteps / 2;
		if (startStep == -1) {
			splitPoint = totalSteps;
			isRoom = true;
			startStep = 0;
		}

		for (int step = startStep; step < totalSteps; step++) {
			// Modulate radius: wider in the middle of the tunnel
			double progress = static_cast<double>(step) / static_cast<double>(totalSteps);
			double sinModulation = std::sin(progress * M_PI);
			double horizontalRadius = 1.5 + sinModulation * static_cast<double>(radius);
			double verticalRadius = horizontalRadius * 0.5;

			if (isRoom) {
				horizontalRadius *= 3.0;
				verticalRadius *= 2.0;
			}

			// Advance position along yaw/pitch direction
			float cosYaw = std::cos(yaw);
			float sinYaw = std::sin(yaw);
			float cosPitch = std::cos(pitch);
			float sinPitch = std::sin(pitch);

			startX += static_cast<double>(cosPitch * cosYaw);
			startY += static_cast<double>(sinPitch);
			startZ += static_cast<double>(cosPitch * sinYaw);

			// Pitch tends toward horizontal with random drift
			pitch *= 0.7f;
			pitch += pitchDrift * 0.05f;
			pitchDrift *= 0.8f;
			pitchDrift += (static_cast<float>(random() % 400) / 100.0f - 2.0f) * 0.02f;

			// Yaw drifts randomly
			yaw += yawDrift * 0.05f;
			yawDrift *= 0.5f;
			yawDrift += (static_cast<float>(random() % 400) / 100.0f - 2.0f) * 0.25f;

			// Possible branch point
			if (!isRoom && step == splitPoint && radius > 1.0f && totalSteps > 0) {
				CarveTunnel(static_cast<int64_t>(random()), originChunkX, originChunkZ,
					targetChunkX, targetChunkZ,
					startX, startY, startZ,
					radius * 0.5f, yaw - 1.57f, pitch / 3.0f,
					step, totalSteps, blocks);
				CarveTunnel(static_cast<int64_t>(random()), originChunkX, originChunkZ,
					targetChunkX, targetChunkZ,
					startX, startY, startZ,
					radius * 0.5f, yaw + 1.57f, pitch / 3.0f,
					step, totalSteps, blocks);
				return;
			}

			// Random skip for some variety
			if (!isRoom && random() % 4 == 0) continue;

			// Check if the carved position is close enough to the target chunk
			double distanceX = startX - centerX;
			double distanceZ = startZ - centerZ;
			double remainingSteps = static_cast<double>(totalSteps - step);
			double maxReach = static_cast<double>(radius) + 18.0;

			if (distanceX * distanceX + distanceZ * distanceZ > remainingSteps * remainingSteps + maxReach * maxReach) {
				return;
			}

			// Compute bounding box of the carved sphere within the target chunk
			int minBlockX = static_cast<int>(std::floor(startX - horizontalRadius)) - targetChunkX * 16 - 1;
			int maxBlockX = static_cast<int>(std::floor(startX + horizontalRadius)) - targetChunkX * 16 + 1;
			int minBlockY = std::max(1, static_cast<int>(std::floor(startY - verticalRadius)) - 1);
			int maxBlockY = std::min(248, static_cast<int>(std::floor(startY + verticalRadius)) + 1);
			int minBlockZ = static_cast<int>(std::floor(startZ - horizontalRadius)) - targetChunkZ * 16 - 1;
			int maxBlockZ = static_cast<int>(std::floor(startZ + horizontalRadius)) - targetChunkZ * 16 + 1;

			// Clamp to chunk boundaries
			minBlockX = std::max(minBlockX, 0);
			maxBlockX = std::min(maxBlockX, 15);
			minBlockZ = std::max(minBlockZ, 0);
			maxBlockZ = std::min(maxBlockZ, 15);

			if (minBlockX > maxBlockX || minBlockZ > maxBlockZ) continue;

			// Carve blocks within the ellipsoid
			for (int blockX = minBlockX; blockX <= maxBlockX; blockX++) {
				double worldBlockX = static_cast<double>(blockX + targetChunkX * 16) + 0.5;
				double normalizedX = (worldBlockX - startX) / horizontalRadius;

				for (int blockZ = minBlockZ; blockZ <= maxBlockZ; blockZ++) {
					double worldBlockZ = static_cast<double>(blockZ + targetChunkZ * 16) + 0.5;
					double normalizedZ = (worldBlockZ - startZ) / horizontalRadius;

					double horizontalDistSq = normalizedX * normalizedX + normalizedZ * normalizedZ;
					if (horizontalDistSq >= 1.0) continue;

					for (int blockY = maxBlockY; blockY >= minBlockY; blockY--) {
						double normalizedY = (static_cast<double>(blockY) + 0.5 - startY) / verticalRadius;
						double totalDistSq = horizontalDistSq + normalizedY * normalizedY;

						if (totalDistSq >= 1.0) continue;

						int index = BlockIndex(blockX, blockY, blockZ);
						int32_t currentBlock = blocks[index];

						// Don't carve into water
						if (currentBlock == BlockState::Water) continue;

						// Check for water above before carving
						if (blockY < 255 && blocks[BlockIndex(blockX, blockY + 1, blockZ)] == BlockState::Water) continue;

						if (!IsBlockCarveTarget(currentBlock)) continue;

						// Below lava level: place lava instead of air
						if (blockY <= LavaLevel) {
							blocks[index] = BlockState::Lava;
						} else {
							blocks[index] = BlockState::Air;
						}

						// If we carved a block that had grass above, replace with dirt
						if (blockY < 255) {
							int aboveIndex = BlockIndex(blockX, blockY + 1, blockZ);
							if (blocks[aboveIndex] == BlockState::GrassBlock) {
								blocks[aboveIndex] = BlockState::Dirt;
							}
						}
					}
				}
			}
		}
	}

	bool CaveGenerator::IsBlockCarveTarget(int32_t stateId) {
		return stateId == BlockState::Stone ||
			stateId == BlockState::Dirt ||
			stateId == BlockState::GrassBlock ||
			stateId == BlockState::Sand ||
			stateId == BlockState::Gravel ||
			stateId == BlockState::Sandstone ||
			stateId == BlockState::CoarseDirt ||
			stateId == BlockState::Podzol ||
			stateId == BlockState::CoalOre ||
			stateId == BlockState::IronOre ||
			stateId == BlockState::GoldOre ||
			stateId == BlockState::RedstoneOre ||
			stateId == BlockState::DiamondOre ||
			stateId == BlockState::LapisOre ||
			stateId == BlockState::CopperOre;
	}

}
