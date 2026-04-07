#pragma once

#include "Axiom/Environment/World/Generator/ChunkGenerator.h"
#include "Axiom/Environment/World/Generator/BiomeProvider.h"
#include "Axiom/Environment/World/Generator/BlockStates.h"
#include "Axiom/Environment/World/Generator/CaveCarver.h"
#include "Axiom/Environment/World/Generator/DensityProvider.h"
#include "Axiom/Environment/World/Generator/Noise.h"
#include "Axiom/Environment/World/Generator/OreDistributor.h"
#include "Axiom/Environment/World/Generator/SurfaceDecorator.h"
#include "Axiom/Environment/World/Generator/TreePlacer.h"
#include "Axiom/Environment/World/ChunkEncoder.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace Axiom {

	/**
	 * Generates terrain using 3D density functions for natural
	 * overhangs, cliffs, and varied terrain shapes.
	 */
	class TerrainChunkGenerator : public ChunkGenerator {
	public:
		static constexpr int SeaLevel = 62;
		static constexpr int MinY = -64;
		static constexpr int MaxY = 319;
		static constexpr int WorldHeight = MaxY - MinY + 1;

		explicit TerrainChunkGenerator(uint64_t seed = 12345);
		ChunkData Generate(int32_t chunkX, int32_t chunkZ) override;
		double SpawnY() const override;

	private:
		bool IsRiver(int worldX, int worldZ) const;

		void FillTerrainDensity(int32_t chunkX, int32_t chunkZ,
			std::array<int, 256>& surfaceHeightmap,
			const std::array<BiomeType, 256>& biomeMap,
			const std::array<bool, 256>& riverMap,
			std::vector<int32_t>& columnBlocks) const;

		void ApplySurfaceBlocks(int32_t chunkX, int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap,
			const std::array<BiomeType, 256>& biomeMap,
			const std::array<bool, 256>& riverMap,
			std::vector<int32_t>& columnBlocks) const;

		void PlaceVegetation(int32_t chunkX, int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap,
			const std::array<BiomeType, 256>& biomeMap,
			std::vector<int32_t>& columnBlocks) const;

		static void EncodeSection(NetworkBuffer& buffer, int sectionMinY,
			const std::vector<int32_t>& columnBlocks, int32_t biomeId);

		DensityProvider m_DensityProvider;
		PerlinNoise m_RiverNoise;
		BiomeProvider m_BiomeProvider;
		CaveCarver m_CaveCarver;
		OreDistributor m_OreDistributor;
		SurfaceDecorator m_SurfaceDecorator;
		TreePlacer m_TreePlacer;
	};

}
