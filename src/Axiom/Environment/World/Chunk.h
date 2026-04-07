#pragma once

#include "Axiom/Core/Base.h"

#include <array>
#include <cstdint>

namespace Axiom {

	/**
	 * A 16x384x16 column of blocks in the world.
	 * Stores block state IDs in sections (16x16x16 each).
	 * The overworld has 24 sections from y=-64 to y=319.
	 */
	class Chunk {
	public:
		static constexpr int SectionCount = 24;
		static constexpr int SectionSize = 16;
		static constexpr int MinY = -64;
		static constexpr int MaxY = 319;
		static constexpr int BlocksPerSection = SectionSize * SectionSize * SectionSize;

		Chunk(int32_t chunkX, int32_t chunkZ)
			: m_ChunkX(chunkX)
			, m_ChunkZ(chunkZ) {}

		int32_t ChunkX() const { return m_ChunkX; }
		int32_t ChunkZ() const { return m_ChunkZ; }

		/**
		 * Get a block state ID at the given world coordinates.
		 */
		int32_t GetBlockState(int x, int y, int z) const {
			int sectionIndex = (y - MinY) / SectionSize;
			if (sectionIndex < 0 || sectionIndex >= SectionCount) return 0;

			int localX = x & 0xF;
			int localY = (y - MinY) & 0xF;
			int localZ = z & 0xF;

			return m_Sections[sectionIndex][localY * 256 + localZ * 16 + localX];
		}

		/**
		 * Set a block state ID at the given world coordinates.
		 */
		void SetBlockState(int x, int y, int z, int32_t stateId) {
			int sectionIndex = (y - MinY) / SectionSize;
			if (sectionIndex < 0 || sectionIndex >= SectionCount) return;

			int localX = x & 0xF;
			int localY = (y - MinY) & 0xF;
			int localZ = z & 0xF;

			m_Sections[sectionIndex][localY * 256 + localZ * 16 + localX] = stateId;
		}

		/**
		 * Get the biome ID for this chunk (currently uniform per chunk).
		 */
		int32_t BiomeId() const { return m_BiomeId; }
		void SetBiomeId(int32_t biomeId) { m_BiomeId = biomeId; }

	private:
		int32_t m_ChunkX;
		int32_t m_ChunkZ;
		int32_t m_BiomeId = 0;
		std::array<std::array<int32_t, BlocksPerSection>, SectionCount> m_Sections{};
	};

}
