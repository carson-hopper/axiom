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
		int32_t heightmapValue;   // Uniform height for all columns
		int32_t biomeId;
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
