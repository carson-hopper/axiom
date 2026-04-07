#pragma once

#include "Axiom/Environment/Level/Generator/ChunkGenerator.h"
#include "Axiom/Environment/Level/Generator/AquiferSampler.h"
#include "Axiom/Environment/Level/Generator/CaveCarver.h"
#include "Axiom/Environment/Level/Generator/CellInterpolator.h"
#include "Axiom/Environment/Level/Generator/ClimateSampler.h"
#include "Axiom/Environment/Level/Generator/NormalNoise.h"
#include "Axiom/Environment/Level/Generator/OreDistributor.h"
#include "Axiom/Environment/Level/Generator/SurfaceDecorator.h"
#include "Axiom/Environment/Level/Generator/TreePlacer.h"
#include "Axiom/Environment/Level/Generator/WaterDecorator.h"
#include "Axiom/Environment/Level/ChunkEncoder.h"

#include <array>
#include <vector>

namespace Axiom {

	/**
	 * Full vanilla-style terrain generator using:
	 * - 3D density with cell interpolation (4x8x4)
	 * - Spline-based terrain shaping from climate parameters
	 * - 4 cave types (cheese, spaghetti, noodle, entrance)
	 * - Aquifer system for underground water/lava
	 * - 6D multi-noise biome selection
	 * - Noise-varied surface blocks and vegetation
	 */
	class TerrainChunkGenerator : public ChunkGenerator {
	public:
		static constexpr int SeaLevel = 63;
		static constexpr int MinY = -64;
		static constexpr int MaxY = 319;
		static constexpr int WorldHeight = MaxY - MinY + 1;

		explicit TerrainChunkGenerator(uint64_t seed = 12345);
		ChunkData Generate(int32_t chunkX, int32_t chunkZ) override;
		double SpawnY() const override;
		int32_t GetBlockAt(int32_t worldX, int32_t worldY, int32_t worldZ) const override;

	private:
		double SampleTerrainDensity(int worldX, int worldY, int worldZ) const;
		int FindSurfaceHeight(const std::vector<int32_t>& columnBlocks, int localX, int localZ) const;

		void FillBlocksFromDensity(int32_t chunkX, int32_t chunkZ,
			const CellInterpolator& interpolator,
			std::array<int, 256>& surfaceHeightmap,
			std::vector<int32_t>& columnBlocks) const;

		void ApplySurfaceBlocks(int32_t chunkX, int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap,
			std::vector<int32_t>& columnBlocks) const;

		void PlaceVegetation(int32_t chunkX, int32_t chunkZ,
			const std::array<int, 256>& surfaceHeightmap,
			std::vector<int32_t>& columnBlocks) const;

		void ComputeBiomeGrid(int32_t chunkX, int32_t chunkZ,
			std::array<int32_t, 16>& biomeGrid) const;

		static void EncodeSection(NetworkBuffer& buffer, int sectionMinY,
			const std::vector<int32_t>& columnBlocks,
			const std::array<int32_t, 16>& biomeGrid);

		ClimateSampler m_ClimateSampler;
		CaveCarver m_CaveCarver;
		AquiferSampler m_AquiferSampler;
		OreDistributor m_OreDistributor;
		SurfaceDecorator m_SurfaceDecorator;
		TreePlacer m_TreePlacer;
		WaterDecorator m_WaterDecorator;
		NormalNoise m_OffsetNoise;
		PerlinNoise m_RiverNoise;
	};

}
