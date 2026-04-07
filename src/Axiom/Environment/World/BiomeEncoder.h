#pragma once

#include "Axiom/Network/NetworkBuffer.h"

#include <array>
#include <cstdint>
#include <vector>

namespace Axiom {

	/**
	 * Encodes biome data for a chunk section at 4x4x4 resolution.
	 * Each section has 4x4x4 = 64 biome entries.
	 *
	 * The biome paletted container uses the same format as blocks:
	 * bits_per_entry=0 for single-valued, 1-3 for indirect palette,
	 * >=4 for direct palette (6 bits).
	 */
	class BiomeEncoder {
	public:
		/**
		 * Encode a biome palette for 64 biome entries (4x4x4).
		 * Entries are indexed as: y*16 + z*4 + x (where x,z,y are 0-3).
		 */
		static void EncodeBiomes(NetworkBuffer& buffer, const std::array<int32_t, 64>& biomeEntries) {
			// Build palette
			std::vector<int32_t> palette;
			std::array<int32_t, 64> paletteIndices{};

			for (int entryIndex = 0; entryIndex < 64; entryIndex++) {
				const int32_t biomeId = biomeEntries[entryIndex];
				int32_t paletteIndex = -1;

				for (int32_t searchIndex = 0; searchIndex < static_cast<int32_t>(palette.size()); searchIndex++) {
					if (palette[searchIndex] == biomeId) {
						paletteIndex = searchIndex;
						break;
					}
				}

				if (paletteIndex == -1) {
					paletteIndex = static_cast<int32_t>(palette.size());
					palette.push_back(biomeId);
				}

				paletteIndices[entryIndex] = paletteIndex;
			}

			// Single biome — use single-valued palette
			if (palette.size() == 1) {
				buffer.WriteByte(0);
				buffer.WriteVarInt(palette[0]);
				return;
			}

			// Indirect palette: minimum 1 bit per entry for biomes
			int bitsPerEntry = 1;
			while ((1 << bitsPerEntry) < static_cast<int>(palette.size())) {
				bitsPerEntry++;
			}
			// Biome palette max is 3 bits for indirect; >=4 is direct (6 bits)
			if (bitsPerEntry > 3) bitsPerEntry = 6;

			buffer.WriteByte(static_cast<uint8_t>(bitsPerEntry));

			// Write palette (only for indirect, not for direct)
			if (bitsPerEntry <= 3) {
				buffer.WriteVarInt(static_cast<int32_t>(palette.size()));
				for (const int32_t biomeId : palette) {
					buffer.WriteVarInt(biomeId);
				}
			}

			// Pack entries into longs
			const int entriesPerLong = 64 / bitsPerEntry;
			const int longCount = (64 + entriesPerLong - 1) / entriesPerLong;

			for (int longIndex = 0; longIndex < longCount; longIndex++) {
				int64_t packed = 0;
				const int startEntry = longIndex * entriesPerLong;

				for (int offset = 0; offset < entriesPerLong; offset++) {
					const int entryIndex = startEntry + offset;
					if (entryIndex >= 64) break;

					int32_t value;
					if (bitsPerEntry <= 3) {
						value = paletteIndices[entryIndex];
					} else {
						// Direct mode: write the biome ID directly
						value = biomeEntries[entryIndex];
					}
					packed |= (static_cast<int64_t>(value) << (offset * bitsPerEntry));
				}

				buffer.WriteLong(packed);
			}
		}

		/**
		 * Encode a single biome for the whole section.
		 */
		static void EncodeSingleBiome(NetworkBuffer& buffer, const int32_t biomeId) {
			buffer.WriteByte(0);
			buffer.WriteVarInt(biomeId);
		}
	};

}
