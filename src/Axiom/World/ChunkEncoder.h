#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>
#include <vector>

namespace Axiom {

	/**
	 * Block state IDs for MC 26.1 (from extractor).
	 */
	namespace BlockState {
		constexpr int32_t Air = 0;
		constexpr int32_t Stone = 1;
		constexpr int32_t GrassBlock = 8;
		constexpr int32_t Dirt = 10;
		constexpr int32_t Bedrock = 85;
	}

	/**
	 * Encodes chunk section and full chunk data for the LevelChunkWithLight packet.
	 */
	class ChunkEncoder {
	public:
		/**
		 * Encode a single chunk section (16x16x16 blocks) as a single block type.
		 * Uses single-valued palette (bits per entry = 0).
		 */
		static void EncodeSingleBlockSection(NetworkBuffer& buffer, int32_t blockStateId) {
			// Block count (non-air blocks in this section)
			buffer.WriteShort(blockStateId == BlockState::Air ? 0 : 4096);

			// Block states — single-valued palette
			buffer.WriteByte(0);  // Bits per entry = 0
			buffer.WriteVarInt(blockStateId);  // The single palette entry
			buffer.WriteVarInt(0);  // Data array length = 0 (no data needed for single value)

			// Biomes — single-valued palette (plains = 0)
			buffer.WriteByte(0);   // Bits per entry = 0
			buffer.WriteVarInt(0); // Biome ID 0 (plains)
			buffer.WriteVarInt(0); // Data array length = 0
		}

		/**
		 * Encode a flat world chunk (bedrock at y=-64, dirt, grass on top, air above).
		 * Overworld: 24 sections (y -64 to 319, each 16 blocks tall).
		 */
		static std::vector<uint8_t> EncodeFlatChunk() {
			NetworkBuffer chunkData;

			// Section 0 (y = -64 to -49): bedrock
			EncodeSingleBlockSection(chunkData, BlockState::Bedrock);

			// Section 1 (y = -48 to -33): stone
			EncodeSingleBlockSection(chunkData, BlockState::Stone);

			// Section 2 (y = -32 to -17): stone
			EncodeSingleBlockSection(chunkData, BlockState::Stone);

			// Section 3 (y = -16 to -1): dirt with grass on top layer
			// For simplicity, just use dirt for the whole section
			EncodeSingleBlockSection(chunkData, BlockState::Dirt);

			// Section 4 (y = 0 to 15): air (but y=0 should be grass)
			// We'll put grass_block as a full section at y=0 for now
			// Actually, for a proper flat world, let's just have it be air from here
			EncodeSingleBlockSection(chunkData, BlockState::Air);

			// Sections 5-23 (y = 16 to 319): air
			for (int section = 5; section < 24; section++) {
				EncodeSingleBlockSection(chunkData, BlockState::Air);
			}

			return std::move(chunkData.Data());
		}

		/**
		 * Encode a LevelChunkWithLight packet payload for a flat chunk.
		 */
		static void EncodeLevelChunkWithLight(NetworkBuffer& packet, int32_t chunkX, int32_t chunkZ) {
			packet.WriteInt(chunkX);
			packet.WriteInt(chunkZ);

			// Heightmap NBT (empty compound — minimal valid NBT)
			// Network NBT: type byte (compound) + end tag
			packet.WriteByte(0x0A);  // TAG_Compound
			packet.WriteByte(0x00);  // TAG_End

			// Chunk data
			auto chunkData = EncodeFlatChunk();
			packet.WriteVarInt(static_cast<int32_t>(chunkData.size()));
			packet.WriteBytes(chunkData);

			// Block entities count
			packet.WriteVarInt(0);

			// Light data
			// Sky light mask (BitSet): length=0
			packet.WriteVarInt(0);
			// Block light mask (BitSet): length=0
			packet.WriteVarInt(0);
			// Empty sky light mask: length=0
			packet.WriteVarInt(0);
			// Empty block light mask: length=0
			packet.WriteVarInt(0);
			// Sky light arrays: count=0
			packet.WriteVarInt(0);
			// Block light arrays: count=0
			packet.WriteVarInt(0);
		}
	};

}
