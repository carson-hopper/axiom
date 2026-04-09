#pragma once

#include "Axiom/Environment/Level/Generator/ChunkGenerator.h"
#include "Axiom/Environment/Level/ChunkEncoder.h"

namespace Axiom {

	/**
	 * Generates classic superflat chunks:
	 *   y=-64: bedrock
	 *   y=-63: dirt
	 *   y=-62: dirt
	 *   y=-61: grass_block
	 *   y=-60+: air
	 */
	class FlatChunkGenerator : public ChunkGenerator {
	public:
		explicit FlatChunkGenerator(const int32_t biomeId = Biome::Plains)
			: m_BiomeId(biomeId) {}

		ChunkData Generate(const int32_t chunkX, const int32_t chunkZ) override {
			return ChunkData{
				chunkX,
				chunkZ,
				ChunkEncoder::EncodeSuperflat(m_BiomeId),
				4,  // Top solid at y=-61, height = 4 blocks above min_y
				m_BiomeId
			};
		}

		double SpawnY() const override { return -30.0; }

	private:
		int32_t m_BiomeId;
	};

}
