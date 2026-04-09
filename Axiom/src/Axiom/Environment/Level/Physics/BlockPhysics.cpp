#include "axpch.h"
#include "Axiom/Environment/Level/Physics/BlockPhysics.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	void BlockPhysics::ScheduleUpdate(const int32_t blockX, const int32_t blockY, const int32_t blockZ) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_PendingUpdates.insert({blockX, blockY, blockZ});

		// Also schedule neighbors
		m_PendingUpdates.insert({blockX + 1, blockY, blockZ});
		m_PendingUpdates.insert({blockX - 1, blockY, blockZ});
		m_PendingUpdates.insert({blockX, blockY + 1, blockZ});
		m_PendingUpdates.insert({blockX, blockY - 1, blockZ});
		m_PendingUpdates.insert({blockX, blockY, blockZ + 1});
		m_PendingUpdates.insert({blockX, blockY, blockZ - 1});
	}

	void BlockPhysics::Tick(const BlockGetter& getBlock, const BlockSetter& setBlock,
		const BlockChangeCallback& onBlockChange) {

		std::unordered_set<BlockPosition, BlockPosition::Hash> toProcess;
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			toProcess = std::move(m_PendingUpdates);
			m_PendingUpdates.clear();
		}

		if (toProcess.empty()) return;

		// Limit updates per tick to prevent lag
		int updateCount = 0;
		constexpr int MaxUpdatesPerTick = 1024;

		for (const auto& position : toProcess) {
			if (updateCount >= MaxUpdatesPerTick) {
				// Re-queue remaining
				std::lock_guard<std::mutex> lock(m_Mutex);
				m_PendingUpdates.insert(position);
				continue;
			}

			const int32_t currentState = getBlock(position.x, position.y, position.z);

			if (FluidState::IsFluid(currentState)) {
				ProcessFluidTick(position.x, position.y, position.z, currentState,
					getBlock, setBlock, onBlockChange);
				updateCount++;
			} else if (GravityBlocks::IsGravityBlock(currentState)) {
				ProcessGravityTick(position.x, position.y, position.z, currentState,
					getBlock, setBlock, onBlockChange);
				updateCount++;
			}
		}

		// Merge next-tick updates
		if (!m_NextUpdates.empty()) {
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_PendingUpdates.insert(m_NextUpdates.begin(), m_NextUpdates.end());
			m_NextUpdates.clear();
		}
	}

	void BlockPhysics::ProcessFluidTick(const int32_t blockX, const int32_t blockY, const int32_t blockZ,
		const int32_t currentState, const BlockGetter& getBlock,
		const BlockSetter& setBlock, const BlockChangeCallback& onBlockChange) {

		// Re-read to guard against stale state from same-tick batch
		const int32_t actualState = getBlock(blockX, blockY, blockZ);
		if (actualState != currentState) return;
        
		const bool isWater = FluidState::IsWater(currentState);
		const int currentLevel = FluidState::GetLevel(currentState);
		const int maxDistance = isWater ? FluidState::WaterMaxDistance : FluidState::LavaMaxDistance;

		static constexpr int DirectionX[] = {1, -1, 0, 0};
		static constexpr int DirectionZ[] = {0, 0, 1, -1};

		// Helper: can this fluid spread into the given block?
		auto canSpreadInto = [&](int32_t targetState, int spreadLevel) -> bool {
			if (targetState == BlockState::Air) return true;
			// Flowing fluid of the same type at a weaker (higher number) level
			if (isWater && FluidState::IsWater(targetState) && FluidState::GetLevel(targetState) > spreadLevel)
				return true;
			if (!isWater && FluidState::IsLava(targetState) && FluidState::GetLevel(targetState) > spreadLevel)
				return true;
			return false;
		};

		// ---- Source blocks (level 0) ----
		if (currentLevel == 0) {
			// Flow down (does NOT block sideways flow)
			const int32_t below = getBlock(blockX, blockY - 1, blockZ);
			if (below == BlockState::Air || FluidState::IsFluid(below)) {
				const int32_t fallingState = isWater
					? FluidState::WaterWithLevel(8)
					: FluidState::LavaWithLevel(8);
				if (below != fallingState) {
					setBlock(blockX, blockY - 1, blockZ, fallingState);
					onBlockChange(blockX, blockY - 1, blockZ, fallingState);
					m_NextUpdates.insert({blockX, blockY - 1, blockZ});
				}
			}

			// Flow sideways at level 1 (always, even if also flowing down)
			const int spreadLevel = 1;
			const int32_t spreadState = isWater
				? FluidState::WaterWithLevel(spreadLevel)
				: FluidState::LavaWithLevel(spreadLevel);

			for (int direction = 0; direction < 4; direction++) {
				const int neighborX = blockX + DirectionX[direction];
				const int neighborZ = blockZ + DirectionZ[direction];
				const int32_t neighborState = getBlock(neighborX, blockY, neighborZ);

				if (canSpreadInto(neighborState, spreadLevel)) {
					setBlock(neighborX, blockY, neighborZ, spreadState);
					onBlockChange(neighborX, blockY, neighborZ, spreadState);
					m_NextUpdates.insert({neighborX, blockY, neighborZ});
				}
			}
			return;
		}

		// ---- Flowing fluid: check support ----
		bool hasSupport = false;

		// Supported from above (falling fluid column)
		const int32_t above = getBlock(blockX, blockY + 1, blockZ);
		if ((isWater && FluidState::IsWater(above)) || (!isWater && FluidState::IsLava(above))) {
			hasSupport = true;
		}

		// Supported by a horizontal neighbor at a stronger (lower number) level
		if (!hasSupport) {
			for (int direction = 0; direction < 4; direction++) {
				const int32_t neighborState = getBlock(
					blockX + DirectionX[direction], blockY, blockZ + DirectionZ[direction]);

				if ((isWater && FluidState::IsWater(neighborState)) || (!isWater && FluidState::IsLava(neighborState))) {
					if (FluidState::GetLevel(neighborState) < currentLevel) {
						hasSupport = true;
						break;
					}
				}
			}
		}

		if (!hasSupport) {
			setBlock(blockX, blockY, blockZ, BlockState::Air);
			onBlockChange(blockX, blockY, blockZ, BlockState::Air);
			return;
		}

		// ---- Continue flowing ----
		if (currentLevel < maxDistance) {
			// Flow down
			const int32_t below = getBlock(blockX, blockY - 1, blockZ);
			if (below == BlockState::Air) {
				const int32_t fallingState = isWater
					? FluidState::WaterWithLevel(8)
					: FluidState::LavaWithLevel(8);
				setBlock(blockX, blockY - 1, blockZ, fallingState);
				onBlockChange(blockX, blockY - 1, blockZ, fallingState);
				m_NextUpdates.insert({blockX, blockY - 1, blockZ});
			}

			// Flow sideways at next level (always, even if also flowing down)
			const int nextLevel = currentLevel + 1;
			if (nextLevel <= maxDistance) {
				const int32_t nextState = isWater
					? FluidState::WaterWithLevel(nextLevel)
					: FluidState::LavaWithLevel(nextLevel);

				for (int direction = 0; direction < 4; direction++) {
					const int neighborX = blockX + DirectionX[direction];
					const int neighborZ = blockZ + DirectionZ[direction];
					const int32_t neighborState = getBlock(neighborX, blockY, neighborZ);

					if (canSpreadInto(neighborState, nextLevel)) {
						setBlock(neighborX, blockY, neighborZ, nextState);
						onBlockChange(neighborX, blockY, neighborZ, nextState);
						m_NextUpdates.insert({neighborX, blockY, neighborZ});
					}
				}
			}
		}
	}

	void BlockPhysics::ProcessGravityTick(const int32_t blockX, const int32_t blockY, const int32_t blockZ,
		const int32_t currentState, const BlockGetter& getBlock,
		const BlockSetter& setBlock, const BlockChangeCallback& onBlockChange) {

		// Re-read the block to confirm it hasn't already moved this tick
		// (ScheduleUpdate queues neighbors, so this position may have been
		// overwritten by an earlier gravity fall in the same tick batch)
		const int32_t actualState = getBlock(blockX, blockY, blockZ);
		if (actualState != currentState) return;

		// Don't fall below the world floor
		if (blockY <= -64) return;

		const int32_t below = getBlock(blockX, blockY - 1, blockZ);

		if (below == BlockState::Air || FluidState::IsFluid(below)) {
			// Fall one block
			setBlock(blockX, blockY, blockZ, BlockState::Air);
			onBlockChange(blockX, blockY, blockZ, BlockState::Air);

			setBlock(blockX, blockY - 1, blockZ, currentState);
			onBlockChange(blockX, blockY - 1, blockZ, currentState);

			// Schedule the new position for NEXT tick only (not this tick)
			m_NextUpdates.insert({blockX, blockY - 1, blockZ});
		}
	}

	bool BlockPhysics::CanFlowInto(const int32_t stateId) const {
		return stateId == BlockState::Air;
	}

}
