#include "BlockPhysics.h"

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

		const bool isWater = FluidState::IsWater(currentState);
		const int currentLevel = FluidState::GetLevel(currentState);
		const int maxDistance = isWater ? FluidState::WaterMaxDistance : FluidState::LavaMaxDistance;

		// Source blocks don't decay
		if (currentLevel == 0) {
			// Flow down
			const int32_t below = getBlock(blockX, blockY - 1, blockZ);
			if (CanFlowInto(below)) {
				// Falling fluid: level 8
				const int32_t fallingState = isWater
					? FluidState::WaterWithLevel(8)
					: FluidState::LavaWithLevel(8);
				setBlock(blockX, blockY - 1, blockZ, fallingState);
				onBlockChange(blockX, blockY - 1, blockZ, fallingState);
				m_NextUpdates.insert({blockX, blockY - 1, blockZ});
				return;
			}

			// Flow sideways
			const int spreadLevel = 1;
			const int32_t spreadState = isWater
				? FluidState::WaterWithLevel(spreadLevel)
				: FluidState::LavaWithLevel(spreadLevel);

			static constexpr int DirectionX[] = {1, -1, 0, 0};
			static constexpr int DirectionZ[] = {0, 0, 1, -1};

			for (int direction = 0; direction < 4; direction++) {
				const int neighborX = blockX + DirectionX[direction];
				const int neighborZ = blockZ + DirectionZ[direction];
				const int32_t neighborState = getBlock(neighborX, blockY, neighborZ);

				if (CanFlowInto(neighborState)) {
					setBlock(neighborX, blockY, neighborZ, spreadState);
					onBlockChange(neighborX, blockY, neighborZ, spreadState);
					m_NextUpdates.insert({neighborX, blockY, neighborZ});
				}
			}
			return;
		}

		// Flowing fluid: check if it should continue flowing or decay

		// Check if still supported by a source or higher-level fluid
		bool hasSource = false;

		// Check above for falling source
		const int32_t above = getBlock(blockX, blockY + 1, blockZ);
		if (FluidState::IsFluid(above) && ((isWater && FluidState::IsWater(above)) || (!isWater && FluidState::IsLava(above)))) {
			hasSource = true;
		}

		// Check horizontal neighbors for higher-level fluid
		if (!hasSource) {
			static constexpr int DirectionX[] = {1, -1, 0, 0};
			static constexpr int DirectionZ[] = {0, 0, 1, -1};

			for (int direction = 0; direction < 4; direction++) {
				const int32_t neighborState = getBlock(
					blockX + DirectionX[direction], blockY, blockZ + DirectionZ[direction]);

				if ((isWater && FluidState::IsWater(neighborState)) || (!isWater && FluidState::IsLava(neighborState))) {
					const int neighborLevel = FluidState::GetLevel(neighborState);
					if (neighborLevel < currentLevel) {
						hasSource = true;
						break;
					}
				}
			}
		}

		if (!hasSource) {
			// Decay: remove this flowing block
			setBlock(blockX, blockY, blockZ, BlockState::Air);
			onBlockChange(blockX, blockY, blockZ, BlockState::Air);
			return;
		}

		// Continue flowing if level < max
		if (currentLevel < maxDistance) {
			// Flow down first
			const int32_t below = getBlock(blockX, blockY - 1, blockZ);
			if (CanFlowInto(below)) {
				const int32_t fallingState = isWater
					? FluidState::WaterWithLevel(8)
					: FluidState::LavaWithLevel(8);
				setBlock(blockX, blockY - 1, blockZ, fallingState);
				onBlockChange(blockX, blockY - 1, blockZ, fallingState);
				m_NextUpdates.insert({blockX, blockY - 1, blockZ});
				return;
			}

			// Flow sideways at next level
			const int nextLevel = currentLevel + 1;
			if (nextLevel <= maxDistance) {
				const int32_t nextState = isWater
					? FluidState::WaterWithLevel(nextLevel)
					: FluidState::LavaWithLevel(nextLevel);

				static constexpr int DirectionX[] = {1, -1, 0, 0};
				static constexpr int DirectionZ[] = {0, 0, 1, -1};

				for (int direction = 0; direction < 4; direction++) {
					const int neighborX = blockX + DirectionX[direction];
					const int neighborZ = blockZ + DirectionZ[direction];
					const int32_t neighborState = getBlock(neighborX, blockY, neighborZ);

					if (CanFlowInto(neighborState)) {
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

		const int32_t below = getBlock(blockX, blockY - 1, blockZ);

		if (below == BlockState::Air || FluidState::IsFluid(below)) {
			// Fall one block
			setBlock(blockX, blockY, blockZ, BlockState::Air);
			onBlockChange(blockX, blockY, blockZ, BlockState::Air);

			setBlock(blockX, blockY - 1, blockZ, currentState);
			onBlockChange(blockX, blockY - 1, blockZ, currentState);

			// Schedule the new position to check if it should keep falling
			m_NextUpdates.insert({blockX, blockY - 1, blockZ});
		}
	}

	bool BlockPhysics::CanFlowInto(const int32_t stateId) const {
		return stateId == BlockState::Air;
	}

}
