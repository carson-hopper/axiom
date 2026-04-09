#pragma once

#include <vector>

namespace Axiom {

	/**
	 * Chunk data produced by a ChunkGenerator, ready to be sent to clients.
	 */
	struct ChunkData {
		int32_t chunkX;
		int32_t chunkZ;
		std::vector<uint8_t> sectionData;
		int32_t heightmapValue;
		int32_t biomeId;
		// Per-section sky light (24 entries, each 2048 bytes = 4096 nibbles)
		// Empty means "use full sky light for all sections"
		std::vector<std::vector<uint8_t>> skyLight;
	};

	/**
	 * Abstract interface for world chunk generation.
	 * Implement this to create different world types (flat, noise, void, etc.)
	 */
	class ChunkGenerator {
	public:
		virtual ~ChunkGenerator() = default;

		/**
		 * Generate chunk data for the given coordinates.
		 * Called from any thread — must be thread-safe.
		 */
		virtual ChunkData Generate(int32_t chunkX, int32_t chunkZ) = 0;

		/**
		 * The Y coordinate where players should spawn.
		 */
		virtual double SpawnY() const = 0;

		/**
		 * Get the block state at any world position.
		 * Used by physics to query generated terrain.
		 * Default returns Air — generators should override for physics support.
		 */
		virtual int32_t GetBlockAt(int32_t /*worldX*/, int32_t /*worldY*/, int32_t /*worldZ*/) const {
			return 0; // Air
		}
	};

}
