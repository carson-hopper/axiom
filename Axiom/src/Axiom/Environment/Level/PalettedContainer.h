#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Efficient block state storage for chunk sections.
	 * Supports multiple palette modes with automatic
	 * promotion as unique block states increase.
	 *
	 * Palette modes:
	 *   SingleValue -- all entries share one value (0 bpe)
	 *   Linear -- up to 16 entries, 4 bits per entry
	 *   Hashmap -- up to 256 entries, 8 bits per entry
	 *   Global -- direct block state IDs, 15 bits per entry
	 */
	class PalettedContainer {
	public:
		static constexpr int SECTION_SIZE = 16;
		static constexpr int TOTAL_ENTRIES = SECTION_SIZE * SECTION_SIZE * SECTION_SIZE;

		enum class PaletteMode {
			SingleValue,
			Linear,
			Hashmap,
			Global
		};

		PalettedContainer();
		explicit PalettedContainer(int32_t defaultValue);

		/**
		 * Get the block state at (x, y, z)
		 * within the section.
		 */
		int32_t Get(int x, int y, int z) const;

		/**
		 * Set the block state at (x, y, z).
		 * May trigger palette promotion.
		 */
		void Set(int x, int y, int z, int32_t value);

		/**
		 * Fill the entire container
		 * with a single value.
		 */
		void Fill(int32_t value);

		/**
		 * Count unique values currently
		 * stored in the container.
		 */
		int UniqueCount() const;

		/**
		 * Get the current
		 * palette mode.
		 */
		PaletteMode Mode() const { return m_Mode; }

		/**
		 * Serialize to network format
		 * for chunk data packets.
		 */
		void WriteToNetwork(NetworkBuffer& buffer) const;

		/**
		 * Read from network
		 * format.
		 */
		void ReadFromNetwork(NetworkBuffer& buffer);

		/**
		 * Count non-zero entries (blocks that
		 * are not air / default value 0).
		 */
		int NonAirCount() const;

	private:
		static constexpr int LINEAR_MAX_ENTRIES = 16;
		static constexpr int HASHMAP_MAX_ENTRIES = 256;
		static constexpr int LINEAR_BITS_PER_ENTRY = 4;
		static constexpr int HASHMAP_BITS_PER_ENTRY = 8;
		static constexpr int GLOBAL_BITS_PER_ENTRY = 15;

		void PromoteToLinear();
		void PromoteToHashmap();
		void PromoteToGlobal();

		int IndexFor(int x, int y, int z) const;

		int EntriesPerLong() const;
		int DataArrayLength() const;

		void PackEntry(int index, uint64_t value);
		uint64_t UnpackEntry(int index) const;

		int FindOrAddPaletteEntry(int32_t value);

		PaletteMode m_Mode = PaletteMode::SingleValue;
		int32_t m_SingleValue = 0;

		std::vector<int32_t> m_Palette;
		std::vector<uint64_t> m_Data;
		int m_BitsPerEntry = 0;
	};

}
