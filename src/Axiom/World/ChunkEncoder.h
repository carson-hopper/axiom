#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>
#include <vector>

namespace Axiom {

	namespace BlockState {
		constexpr int32_t Air = 0;
		constexpr int32_t Stone = 1;
		constexpr int32_t GrassBlock = 8;
		constexpr int32_t Dirt = 10;
		constexpr int32_t Bedrock = 85;
	}

	/**
	 * Heightmap type IDs matching Heightmap.Types enum ordinals.
	 */
	namespace HeightmapType {
		constexpr int32_t WorldSurfaceWg = 0;
		constexpr int32_t WorldSurface = 1;
		constexpr int32_t OceanFloorWg = 2;
		constexpr int32_t OceanFloor = 3;
		constexpr int32_t MotionBlocking = 4;
		constexpr int32_t MotionBlockingNoLeaves = 5;
	}

	class ChunkEncoder {
	public:
		static void EncodeSingleBlockSection(NetworkBuffer& buffer, int32_t blockStateId) {
			buffer.WriteShort(blockStateId == BlockState::Air ? 0 : 4096);

			// Block states — single-valued palette
			buffer.WriteByte(0);
			buffer.WriteVarInt(blockStateId);
			buffer.WriteVarInt(0);

			// Biomes — single-valued palette (plains = 0)
			buffer.WriteByte(0);
			buffer.WriteVarInt(0);
			buffer.WriteVarInt(0);
		}

		static std::vector<uint8_t> EncodeFlatChunk() {
			NetworkBuffer chunkData;

			EncodeSingleBlockSection(chunkData, BlockState::Bedrock);  // y=-64 to -49
			EncodeSingleBlockSection(chunkData, BlockState::Stone);    // y=-48 to -33
			EncodeSingleBlockSection(chunkData, BlockState::Stone);    // y=-32 to -17
			EncodeSingleBlockSection(chunkData, BlockState::Dirt);     // y=-16 to -1
			EncodeSingleBlockSection(chunkData, BlockState::Air);      // y=0 to 15

			for (int section = 5; section < 24; section++) {
				EncodeSingleBlockSection(chunkData, BlockState::Air);
			}

			return std::move(chunkData.Data());
		}

		/**
		 * Encode heightmap data as Map<HeightmapType, long[]>.
		 * Format: VarInt(mapSize) + for each: VarInt(typeId) + long[]
		 *
		 * long[] is encoded as: VarInt(arrayLength) + arrayLength longs
		 *
		 * For a flat world with top solid block at y=-1 (section 3),
		 * the height value is 64 (blocks above min_y=-64).
		 * 9 bits per entry, 256 entries, packed into longs.
		 */
		static void EncodeHeightmaps(NetworkBuffer& buffer) {
			// We send 2 heightmap types that the client needs: MOTION_BLOCKING and WORLD_SURFACE
			buffer.WriteVarInt(2);

			// For flat world: top solid block at y=-1, height value = -1 - (-64) + 1 = 64
			// Overworld range: -64 to 319, so 384 possible values, needs 9 bits
			// 256 entries * 9 bits = 2304 bits / 64 = 36 longs
			// But the client enforces max 24 longs? Let me use the correct calculation.
			// With height 384, bits per entry = ceil(log2(384+1)) = 9
			// Entries per long = floor(64/9) = 7
			// Number of longs = ceil(256/7) = 37
			// Actually the max the client allows seems to be based on bit packing.
			// Let me just compute it properly.

			constexpr int bitsPerEntry = 9;
			constexpr int entriesPerLong = 64 / bitsPerEntry; // 7
			constexpr int longCount = (256 + entriesPerLong - 1) / entriesPerLong; // 37

			// Height value for flat world: all columns have height 64
			// (top solid at y=-1, which is 64 blocks above min_y=-64)
			constexpr int64_t heightValue = 64;

			// Pack entries into longs
			std::vector<int64_t> packedData(longCount, 0);
			for (int entry = 0; entry < 256; entry++) {
				int longIndex = entry / entriesPerLong;
				int bitOffset = (entry % entriesPerLong) * bitsPerEntry;
				packedData[longIndex] |= (heightValue << bitOffset);
			}

			// MOTION_BLOCKING
			buffer.WriteVarInt(HeightmapType::MotionBlocking);
			buffer.WriteVarInt(static_cast<int32_t>(packedData.size()));
			for (int64_t value : packedData) {
				buffer.WriteLong(value);
			}

			// WORLD_SURFACE
			buffer.WriteVarInt(HeightmapType::WorldSurface);
			buffer.WriteVarInt(static_cast<int32_t>(packedData.size()));
			for (int64_t value : packedData) {
				buffer.WriteLong(value);
			}
		}

		/**
		 * Encode light data for the LevelChunkWithLight packet.
		 * Format matches ClientboundLightUpdatePacketData:
		 *   BitSet skyYMask, BitSet blockYMask,
		 *   BitSet emptySkyYMask, BitSet emptyBlockYMask,
		 *   List<byte[]> skyUpdates, List<byte[]> blockUpdates
		 *
		 * BitSet is encoded as: VarInt(longCount) + longs
		 */
		static void EncodeLightData(NetworkBuffer& buffer) {
			// Sky light mask: no sections have sky light
			buffer.WriteVarInt(0);  // empty BitSet

			// Block light mask: no sections have block light
			buffer.WriteVarInt(0);  // empty BitSet

			// Empty sky light mask: all sections have empty sky light
			buffer.WriteVarInt(0);  // empty BitSet

			// Empty block light mask: all sections have empty block light
			buffer.WriteVarInt(0);  // empty BitSet

			// Sky light updates: count=0
			buffer.WriteVarInt(0);

			// Block light updates: count=0
			buffer.WriteVarInt(0);
		}

		static void EncodeLevelChunkWithLight(NetworkBuffer& packet, int32_t chunkX, int32_t chunkZ) {
			packet.WriteInt(chunkX);
			packet.WriteInt(chunkZ);

			// Heightmaps as Map<HeightmapType, long[]>
			EncodeHeightmaps(packet);

			// Chunk section data
			auto chunkData = EncodeFlatChunk();
			packet.WriteVarInt(static_cast<int32_t>(chunkData.size()));
			packet.WriteBytes(chunkData);

			// Block entities count
			packet.WriteVarInt(0);

			// Light data
			EncodeLightData(packet);
		}
	};

}
