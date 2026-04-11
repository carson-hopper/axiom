#pragma once

#include "Axiom/Utilities/Memory/Ref.h"

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
	 * Level of detail at which a chunk should be generated. Skeletal
	 * chunks skip the expensive cave-carving and feature-decoration
	 * passes — they are used for the "cold" ring of a player's view
	 * distance where the block data will never be interacted with
	 * before it is upgraded to Full.
	 */
	enum class ChunkTier : uint8_t {
		Skeletal = 0,
		Full     = 1,
	};

	/**
	 * Abstract interface for world chunk generation.
	 * Implement this to create different world types (flat, noise, void, etc.)
	 */
	class ChunkGenerator : public virtual RefCounted {
	public:
		virtual ~ChunkGenerator() = default;

		/**
		 * Generate chunk data for the given coordinates at the
		 * requested level of detail. Called from any thread —
		 * must be thread-safe. Implementations that cannot honour
		 * a skeletal tier may simply fall through to the full
		 * path; the tier is a hint, not a hard contract.
		 */
		virtual ChunkData Generate(int32_t chunkX, int32_t chunkZ,
			ChunkTier tier = ChunkTier::Full) = 0;

		/**
		 * Promote an already-generated skeletal chunk to Full
		 * without re-running heightmap/biome work. The default
		 * implementation just calls `Generate(..., Full)` — only
		 * override this if your generator keeps an internal
		 * cache of the skeletal block data that can be mutated
		 * in place by the cave and feature passes. Called from
		 * any thread — must be thread-safe.
		 */
		virtual ChunkData Decorate(int32_t chunkX, int32_t chunkZ) {
			return Generate(chunkX, chunkZ, ChunkTier::Full);
		}

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
