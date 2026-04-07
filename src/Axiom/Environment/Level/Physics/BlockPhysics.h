#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <unordered_set>
#include <vector>

namespace Axiom {

	/**
	 * Block state helpers for fluid levels and gravity blocks.
	 */
	namespace FluidState {
		// Water: states 86 (source, level 0) to 101 (level 15)
		constexpr int32_t WaterSource = 86;
		constexpr int32_t WaterMaxFlowing = 101;

		// Lava: states 102 (source, level 0) to 117 (level 15)
		constexpr int32_t LavaSource = 102;
		constexpr int32_t LavaMaxFlowing = 117;

		inline bool IsWater(const int32_t stateId) {
			return stateId >= WaterSource && stateId <= WaterMaxFlowing;
		}

		inline bool IsLava(const int32_t stateId) {
			return stateId >= LavaSource && stateId <= LavaMaxFlowing;
		}

		inline bool IsFluid(const int32_t stateId) {
			return IsWater(stateId) || IsLava(stateId);
		}

		inline bool IsSource(const int32_t stateId) {
			return stateId == WaterSource || stateId == LavaSource;
		}

		inline int GetLevel(const int32_t stateId) {
			if (IsWater(stateId)) return stateId - WaterSource;
			if (IsLava(stateId)) return stateId - LavaSource;
			return 0;
		}

		inline int32_t WaterWithLevel(const int level) {
			return WaterSource + std::clamp(level, 0, 15);
		}

		inline int32_t LavaWithLevel(const int level) {
			return LavaSource + std::clamp(level, 0, 15);
		}

		// Max flow distance: water=7, lava=3 (overworld)
		constexpr int WaterMaxDistance = 7;
		constexpr int LavaMaxDistance = 3;
	}

	namespace GravityBlocks {
		inline bool IsGravityBlock(const int32_t stateId) {
			return stateId == BlockState::Sand
				|| stateId == BlockState::RedSand
				|| stateId == BlockState::Gravel;
		}
	}

	struct BlockPosition {
		int32_t x, y, z;

		bool operator==(const BlockPosition& other) const {
			return x == other.x && y == other.y && z == other.z;
		}

		struct Hash {
			size_t operator()(const BlockPosition& position) const {
				size_t hash = std::hash<int32_t>{}(position.x);
				hash ^= std::hash<int32_t>{}(position.y) << 1;
				hash ^= std::hash<int32_t>{}(position.z) << 2;
				return hash;
			}
		};
	};

	using BlockChangeCallback = std::function<void(int32_t x, int32_t y, int32_t z, int32_t newState)>;

	/**
	 * Processes block physics: fluid flow and gravity.
	 * Call ScheduleUpdate when a block changes, then call
	 * Tick() every server tick to process pending updates.
	 */
	class BlockPhysics {
	public:
		using BlockGetter = std::function<int32_t(int32_t, int32_t, int32_t)>;
		using BlockSetter = std::function<void(int32_t, int32_t, int32_t, int32_t)>;

		void ScheduleUpdate(const int32_t blockX, const int32_t blockY, const int32_t blockZ);

		void Tick(const BlockGetter& getBlock, const BlockSetter& setBlock,
			const BlockChangeCallback& onBlockChange);

	private:
		void ProcessFluidTick(const int32_t blockX, const int32_t blockY, const int32_t blockZ,
			const int32_t currentState, const BlockGetter& getBlock,
			const BlockSetter& setBlock, const BlockChangeCallback& onBlockChange);

		void ProcessGravityTick(const int32_t blockX, const int32_t blockY, const int32_t blockZ,
			const int32_t currentState, const BlockGetter& getBlock,
			const BlockSetter& setBlock, const BlockChangeCallback& onBlockChange);

		bool CanFlowInto(const int32_t stateId) const;

		std::mutex m_Mutex;
		std::unordered_set<BlockPosition, BlockPosition::Hash> m_PendingUpdates;
		std::unordered_set<BlockPosition, BlockPosition::Hash> m_NextUpdates;
	};

}
