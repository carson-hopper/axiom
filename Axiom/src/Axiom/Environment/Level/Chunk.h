#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Error.h"
#include "Axiom/Core/Tickable.h"
#include "Axiom/Environment/Level/PalettedContainer.h"

#include <array>
#include <cstdint>

namespace Axiom {

	/**
	 * A 16x16x16 section of blocks within a chunk.
	 * Only allocated when a non-air block is placed.
	 * Uses PalettedContainer for efficient storage.
	 */
	class ChunkSection {
	public:
		static constexpr int Size = 16;
		static constexpr int BlockCount = Size * Size * Size;

		ChunkSection() = default;

		int32_t GetBlockState(const int localX, const int localY, const int localZ) const {
			return m_Blocks.Get(localX, localY, localZ);
		}

		void SetBlockState(const int localX, const int localY, const int localZ, const int32_t stateId) {
			m_Blocks.Set(localX, localY, localZ, stateId);
		}

		int NonAirCount() const {
			return m_Blocks.NonAirCount();
		}

		bool IsEmpty() const {
			return m_Blocks.NonAirCount() == 0;
		}

		PalettedContainer& Blocks() { return m_Blocks; }
		const PalettedContainer& Blocks() const { return m_Blocks; }

	private:
		PalettedContainer m_Blocks;
	};

	/**
	 * A 16x384x16 column of blocks in the world.
	 * Sections are lazy-allocated — empty sections use zero memory.
	 * The overworld has 24 sections from y=-64 to y=319.
	 */
	class Chunk : public Tickable {
	public:
		static constexpr int SectionCount = 24;
		static constexpr int SectionSize = 16;
		static constexpr int MinY = -64;
		static constexpr int MaxY = 319;

		Chunk(const int32_t chunkX, const int32_t chunkZ)
			: m_ChunkX(chunkX)
			, m_ChunkZ(chunkZ) {}

		/**
		 * Called every game tick (50ms interval, 20 TPS).
		 * Override to implement chunk-specific ticking logic.
		 */
		void OnTick(Timestep timestep) override;

		int32_t ChunkX() const { return m_ChunkX; }
		int32_t ChunkZ() const { return m_ChunkZ; }

		/**
		 * Get a block state ID at the given world coordinates.
		 * Returns 0 (air) for unallocated sections.
		 */
		int32_t GetBlockState(int x, int y, int z) const {
			const int sectionIndex = (y - MinY) / SectionSize;
			if (sectionIndex < 0 || sectionIndex >= SectionCount) return 0;

			const auto& section = m_Sections[sectionIndex];
			if (!section) return 0;

			return section->GetBlockState(x & 0xF, (y - MinY) & 0xF, z & 0xF);
		}

		/**
		 * Set a block state ID at the given world coordinates.
		 * Allocates the section on first write.
		 *
		 * @param x Block X coordinate (0-15 within chunk)
		 * @param y Block Y coordinate (world coordinates, -64 to 319)
		 * @param z Block Z coordinate (0-15 within chunk)
		 * @param stateId The block state ID to set (0 = air)
		 * @return Result<void> Success or ErrorCode::BlockOutOfBounds if Y is out of range
		 */
		Result<void> SetBlockState(int x, int y, int z, int32_t stateId) {
			int sectionIndex = (y - MinY) / SectionSize;
			if (sectionIndex < 0 || sectionIndex >= SectionCount) {
				return MakeError<void>(ErrorCode::BlockOutOfBounds);
			}

			auto& section = m_Sections[sectionIndex];
			if (!section) {
				if (stateId == 0) return {};  // Don't allocate for air
				section = CreateScope<ChunkSection>();
			}

			section->SetBlockState(x & 0xF, (y - MinY) & 0xF, z & 0xF, stateId);
			return {};
		}

		/**
		 * Get a section pointer (may be null for empty sections).
		 */
		ChunkSection* GetSection(const int index) const {
			if (index < 0 || index >= SectionCount) return nullptr;
			return m_Sections[index].get();
		}

		/**
		 * Ensure a section exists, allocating if needed.
		 *
		 * @param index Section index (0-23)
		 * @return Pointer to the section, or nullptr if index is out of bounds
		 */
		ChunkSection* GetOrCreateSection(const int index) {
			if (index < 0 || index >= SectionCount) {
				return nullptr;
			}
			if (!m_Sections[index]) {
				m_Sections[index] = CreateScope<ChunkSection>();
			}
			return m_Sections[index].get();
		}

		bool HasSection(const int index) const {
			return index >= 0 && index < SectionCount && m_Sections[index] != nullptr;
		}

		int32_t BiomeId() const { return m_BiomeId; }
		void SetBiomeId(const int32_t biomeId) { m_BiomeId = biomeId; }

		/**
		 * Count allocated sections (for memory diagnostics).
		 */
		int AllocatedSectionCount() const {
			int count = 0;
			for (const auto& section : m_Sections) {
				if (section) count++;
			}
			return count;
		}

		/**
		 * Approximate memory usage in bytes.
		 */
		size_t MemoryUsage() const {
			size_t base = sizeof(Chunk);
			for (const auto& section : m_Sections) {
				if (section) base += sizeof(ChunkSection);
			}
			return base;
		}

	private:
		int32_t m_ChunkX;
		int32_t m_ChunkZ;
		int32_t m_BiomeId = 0;
		std::array<Scope<ChunkSection>, SectionCount> m_Sections;
	};

}
