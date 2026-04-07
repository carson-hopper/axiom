#include "TerrainChunkGenerator.h"

namespace Axiom {

	TerrainChunkGenerator::TerrainChunkGenerator(const uint64_t seed)
		: m_ClimateSampler(seed)
		, m_CaveCarver(seed + 1000)
		, m_AquiferSampler(seed + 2000)
		, m_OreDistributor(seed + 3000)
		, m_SurfaceDecorator(seed + 4000)
		, m_TreePlacer(seed + 5000)
		, m_WaterDecorator(seed + 6000)
		, m_OffsetNoise(seed + 7000, NoiseParameters::Offset())
		, m_RiverNoise(seed + 8000) {}

	ChunkData TerrainChunkGenerator::Generate(const int32_t chunkX, const int32_t chunkZ) {
		// Phase 1: Sample density at cell corners (4×8×4 cells)
		CellInterpolator interpolator;
		interpolator.SampleCorners(chunkX, chunkZ,
			[this](const int worldX, const int worldY, const int worldZ) {
				return SampleTerrainDensity(worldX, worldY, worldZ);
			});

		// Phase 2: Fill blocks from interpolated density + aquifer
		std::vector<int32_t> columnBlocks(16 * 16 * WorldHeight, BlockState::Air);
		std::array<int, 256> surfaceHeightmap{};
		FillBlocksFromDensity(chunkX, chunkZ, interpolator, surfaceHeightmap, columnBlocks);

		// Phase 3: Surface decoration
		ApplySurfaceBlocks(chunkX, chunkZ, surfaceHeightmap, columnBlocks);

		// Phase 4: Trees, vegetation, water features
		std::array<BiomeType, 256> biomeMap{};
		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				biomeMap[localZ * 16 + localX] = m_ClimateSampler.SelectBiomeType(
					chunkX * 16 + localX, chunkZ * 16 + localZ);
			}
		}

		m_TreePlacer.PlaceTrees(chunkX, chunkZ, surfaceHeightmap, biomeMap, columnBlocks);
		PlaceVegetation(chunkX, chunkZ, surfaceHeightmap, columnBlocks);
		m_WaterDecorator.Decorate(chunkX, chunkZ, surfaceHeightmap, biomeMap, columnBlocks);

		// Phase 5: Encode sections with per-4x4 biome blending
		std::array<int32_t, 16> biomeGrid{};
		ComputeBiomeGrid(chunkX, chunkZ, biomeGrid);

		NetworkBuffer sectionData;
		for (int sectionIndex = 0; sectionIndex < 24; sectionIndex++) {
			EncodeSection(sectionData, MinY + sectionIndex * 16, columnBlocks, biomeGrid);
		}

		int maximumHeight = MinY;
		for (const int height : surfaceHeightmap) {
			if (height > maximumHeight) maximumHeight = height;
		}

		const int32_t centerBiomeId = m_ClimateSampler.SelectBiome(
			chunkX * 16 + 8, 64, chunkZ * 16 + 8);

		return ChunkData{chunkX, chunkZ, std::move(sectionData.Data()),
			std::max(SeaLevel - MinY + 1, maximumHeight - MinY + 1), centerBiomeId};
	}

	double TerrainChunkGenerator::SpawnY() const {
		return std::max(m_ClimateSampler.ComputeSurfaceY(0, 0), static_cast<double>(SeaLevel)) + 1.0;
	}

	int32_t TerrainChunkGenerator::GetBlockAt(const int32_t worldX, const int32_t worldY,
		const int32_t worldZ) const {

		if (worldY < MinY || worldY > MaxY) return BlockState::Air;

		const double density = SampleTerrainDensity(worldX, worldY, worldZ);
		const double approximateSurfaceY = m_ClimateSampler.ComputeSurfaceY(worldX, worldZ);

		if (density > 0) {
			if (worldY <= MinY + 5) return BlockState::Bedrock;
			if (worldY <= 0) return BlockState::Deepslate;

			// Check if this is a surface block
			const double densityAbove = SampleTerrainDensity(worldX, worldY + 1, worldZ);
			if (densityAbove <= 0) {
				// This is the surface — apply biome surface block
				const BiomeType biome = m_ClimateSampler.SelectBiomeType(worldX, worldZ);
				return m_SurfaceDecorator.GetSurfaceBlock(worldX, worldZ, worldY, biome);
			}

			// Check if near surface (sub-surface)
			const double density3Above = SampleTerrainDensity(worldX, worldY + 4, worldZ);
			if (density3Above <= 0) {
				const BiomeType biome = m_ClimateSampler.SelectBiomeType(worldX, worldZ);
				return m_SurfaceDecorator.GetSubSurfaceBlock(worldX, worldZ, worldY, biome);
			}

			return BlockState::Stone;
		}

		// Non-solid
		if (worldY > SeaLevel) return BlockState::Air;

		const bool isUnderground = worldY < approximateSurfaceY - 2;
		if (isUnderground) {
			if (worldY < AquiferSampler::LavaLevel) return BlockState::Lava;
			return BlockState::Air;
		}

		return BlockState::Water;
	}

	double TerrainChunkGenerator::SampleTerrainDensity(const int worldX, const int worldY,
		const int worldZ) const {

		const double blockX = static_cast<double>(worldX);
		const double blockY = static_cast<double>(worldY);
		const double blockZ = static_cast<double>(worldZ);

		// Get climate-driven target surface height
		const double continentalness = m_ClimateSampler.GetContinentalness(worldX, worldZ);
		const double erosion = m_ClimateSampler.GetErosion(worldX, worldZ);
		const double weirdness = m_ClimateSampler.GetWeirdness(worldX, worldZ);

		const double targetHeight = m_ClimateSampler.ComputeApproximateSurfaceY(
			continentalness, erosion, weirdness);

		// Small-scale offset noise for surface variation
		const double offsetValue = m_OffsetNoise.GetValue(blockX, 0, blockZ) * 3.0;
		const double adjustedTarget = targetHeight + offsetValue;

		// Vertical gradient: the core terrain shape function
		// Very gentle gradient (0.025) with tanh creates the smooth,
		// rounded hills that vanilla MC is known for
		const double heightDelta = adjustedTarget - blockY;
		double density = std::tanh(heightDelta * 0.025);

		// Cave carving: subtract cave density from terrain
		// Store pre-cave density for aquifer to distinguish caves from natural voids
		const double preCaveDensity = density;

		if (worldY > MinY + 5 && worldY < adjustedTarget - 1) {
			const double caveDensity = m_CaveCarver.SampleCaveDensity(worldX, worldY, worldZ);
			density -= caveDensity;
		}

		// Bedrock floor: guaranteed solid
		if (worldY <= MinY + 5) {
			density = 1.0;
		}

		// Squeeze above surface to prevent floating terrain
		if (blockY > adjustedTarget + 6) {
			const double squeezeAmount = (blockY - adjustedTarget - 6) * 0.015;
			density -= squeezeAmount;
		}

		return density;
	}

	void TerrainChunkGenerator::FillBlocksFromDensity(const int32_t chunkX, const int32_t chunkZ,
		const CellInterpolator& interpolator,
		std::array<int, 256>& surfaceHeightmap,
		std::vector<int32_t>& columnBlocks) const {

		surfaceHeightmap.fill(MinY);

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int columnIndex = localZ * 16 + localX;
				const int worldX = chunkX * 16 + localX;
				const int worldZ = chunkZ * 16 + localZ;

				// Compute approximate surface height for this column
				const double approximateSurfaceY = m_ClimateSampler.ComputeSurfaceY(worldX, worldZ);

				for (int worldY = MinY; worldY <= MaxY; worldY++) {
					const double density = interpolator.GetDensity(localX, worldY, localZ);
					const int absoluteY = worldY - MinY;

					if (density > 0) {
						// Solid terrain
						if (worldY <= MinY + 5) {
							columnBlocks[absoluteY * 256 + columnIndex] = BlockState::Bedrock;
						} else if (worldY <= 0) {
							columnBlocks[absoluteY * 256 + columnIndex] = BlockState::Deepslate;
						} else {
							columnBlocks[absoluteY * 256 + columnIndex] = BlockState::Stone;
						}
						surfaceHeightmap[columnIndex] = worldY;
					} else {
						// Non-solid: determine air vs water vs lava
						const bool isUnderground = worldY < approximateSurfaceY - 2;

						if (worldY > SeaLevel) {
							// Above sea level: always air
							columnBlocks[absoluteY * 256 + columnIndex] = BlockState::Air;
						} else if (isUnderground) {
							// Underground cave: air (not water)
							if (worldY < AquiferSampler::LavaLevel) {
								// Deep underground: occasional lava
								columnBlocks[absoluteY * 256 + columnIndex] = BlockState::Lava;
							} else {
								columnBlocks[absoluteY * 256 + columnIndex] = BlockState::Air;
							}
						} else {
							// Ocean/river: water up to sea level
							columnBlocks[absoluteY * 256 + columnIndex] = BlockState::Water;
						}
					}
				}

				// Ore placement pass
				const int surfaceHeight = surfaceHeightmap[columnIndex];
				for (int worldY = MinY + 5; worldY <= surfaceHeight - 2; worldY++) {
					const int absoluteY = worldY - MinY;
					const int32_t currentBlock = columnBlocks[absoluteY * 256 + columnIndex];
					if (currentBlock == BlockState::Stone || currentBlock == BlockState::Deepslate) {
						const int32_t oreBlock = m_OreDistributor.GetOre(
							worldX, worldY, worldZ, currentBlock == BlockState::Deepslate);
						if (oreBlock != 0) columnBlocks[absoluteY * 256 + columnIndex] = oreBlock;
					}
				}
			}
		}
	}

	int TerrainChunkGenerator::FindSurfaceHeight(const std::vector<int32_t>& columnBlocks,
		const int localX, const int localZ) const {

		const int columnIndex = localZ * 16 + localX;
		for (int worldY = MaxY; worldY >= MinY; worldY--) {
			const int32_t block = columnBlocks[(worldY - MinY) * 256 + columnIndex];
			if (block != BlockState::Air && block != BlockState::Water && block != BlockState::Lava) {
				return worldY;
			}
		}
		return MinY;
	}

	void TerrainChunkGenerator::ApplySurfaceBlocks(const int32_t chunkX, const int32_t chunkZ,
		const std::array<int, 256>& surfaceHeightmap,
		std::vector<int32_t>& columnBlocks) const {

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int columnIndex = localZ * 16 + localX;
				const int surfaceHeight = surfaceHeightmap[columnIndex];
				if (surfaceHeight <= MinY) continue;

				const int worldX = chunkX * 16 + localX;
				const int worldZ = chunkZ * 16 + localZ;
				const BiomeType biome = m_ClimateSampler.SelectBiomeType(worldX, worldZ);

				const int surfaceAbsoluteY = surfaceHeight - MinY;
				const int32_t surfaceBlock = columnBlocks[surfaceAbsoluteY * 256 + columnIndex];

				if (surfaceBlock != BlockState::Stone && surfaceBlock != BlockState::Deepslate) continue;

				// Check if river
				const bool isRiver = std::abs(m_RiverNoise.OctaveNoise(
					worldX * 0.004, worldZ * 0.004, 3, 0.5)) < 0.03;

				const int32_t newSurface = isRiver
					? BlockState::Sand
					: m_SurfaceDecorator.GetSurfaceBlock(worldX, worldZ, surfaceHeight, biome);
				columnBlocks[surfaceAbsoluteY * 256 + columnIndex] = newSurface;

				// Sub-surface layers (3-4 blocks deep)
				const int32_t subSurfaceBlock = m_SurfaceDecorator.GetSubSurfaceBlock(
					worldX, worldZ, surfaceHeight, biome);
				for (int depth = 1; depth <= 4; depth++) {
					const int belowY = surfaceHeight - depth;
					if (belowY < MinY) break;
					const int32_t existingBlock = columnBlocks[(belowY - MinY) * 256 + columnIndex];
					if (existingBlock == BlockState::Stone || existingBlock == BlockState::Deepslate) {
						columnBlocks[(belowY - MinY) * 256 + columnIndex] = subSurfaceBlock;
					}
				}
			}
		}
	}

	void TerrainChunkGenerator::PlaceVegetation(const int32_t chunkX, const int32_t chunkZ,
		const std::array<int, 256>& surfaceHeightmap,
		std::vector<int32_t>& columnBlocks) const {

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int columnIndex = localZ * 16 + localX;
				const int surfaceHeight = surfaceHeightmap[columnIndex];
				if (surfaceHeight <= SeaLevel) continue;

				const int surfaceAbsoluteY = surfaceHeight - MinY;
				const int aboveAbsoluteY = surfaceAbsoluteY + 1;
				if (aboveAbsoluteY >= WorldHeight) continue;

				const int32_t surfaceBlock = columnBlocks[surfaceAbsoluteY * 256 + columnIndex];
				const int32_t aboveBlock = columnBlocks[aboveAbsoluteY * 256 + columnIndex];
				if (aboveBlock != BlockState::Air) continue;

				const bool canDecorate = surfaceBlock == BlockState::GrassBlock
					|| surfaceBlock == BlockState::Podzol || surfaceBlock == BlockState::Stone;
				if (!canDecorate) continue;

				const int worldX = chunkX * 16 + localX;
				const int worldZ = chunkZ * 16 + localZ;
				const BiomeType biome = m_ClimateSampler.SelectBiomeType(worldX, worldZ);

				const int32_t vegetation = m_SurfaceDecorator.GetVegetation(worldX, worldZ, biome);
				if (vegetation != BlockState::Air) {
					columnBlocks[aboveAbsoluteY * 256 + columnIndex] = vegetation;
				}
			}
		}
	}

	void TerrainChunkGenerator::ComputeBiomeGrid(const int32_t chunkX, const int32_t chunkZ,
		std::array<int32_t, 16>& biomeGrid) const {

		for (int gridZ = 0; gridZ < 4; gridZ++) {
			for (int gridX = 0; gridX < 4; gridX++) {
				const int worldX = chunkX * 16 + gridX * 4 + 2;
				const int worldZ = chunkZ * 16 + gridZ * 4 + 2;
				biomeGrid[gridZ * 4 + gridX] = m_ClimateSampler.SelectBiome(worldX, 64, worldZ);
			}
		}
	}

	void TerrainChunkGenerator::EncodeSection(NetworkBuffer& buffer, const int sectionMinY,
		const std::vector<int32_t>& columnBlocks,
		const std::array<int32_t, 16>& biomeGrid) {

		std::vector<int32_t> palette;
		std::array<int32_t, 4096> blockIndices{};
		int nonAirCount = 0;

		for (int localY = 0; localY < 16; localY++) {
			const int absoluteY = (sectionMinY - MinY) + localY;
			for (int localZ = 0; localZ < 16; localZ++) {
				for (int localX = 0; localX < 16; localX++) {
					const int32_t blockState = columnBlocks[absoluteY * 256 + localZ * 16 + localX];
					int32_t paletteIndex = -1;
					for (int32_t searchIndex = 0; searchIndex < static_cast<int32_t>(palette.size()); searchIndex++) {
						if (palette[searchIndex] == blockState) { paletteIndex = searchIndex; break; }
					}
					if (paletteIndex == -1) {
						paletteIndex = static_cast<int32_t>(palette.size());
						palette.push_back(blockState);
					}
					blockIndices[localY * 256 + localZ * 16 + localX] = paletteIndex;
					if (blockState != BlockState::Air) nonAirCount++;
				}
			}
		}

		std::array<int32_t, 64> biomeEntries{};
		for (int biomeY = 0; biomeY < 4; biomeY++) {
			for (int biomeZ = 0; biomeZ < 4; biomeZ++) {
				for (int biomeX = 0; biomeX < 4; biomeX++) {
					biomeEntries[biomeY * 16 + biomeZ * 4 + biomeX] = biomeGrid[biomeZ * 4 + biomeX];
				}
			}
		}

		bool singleBiome = true;
		for (int entryIndex = 1; entryIndex < 64; entryIndex++) {
			if (biomeEntries[entryIndex] != biomeEntries[0]) { singleBiome = false; break; }
		}

		if (palette.size() == 1 && singleBiome) {
			ChunkEncoder::EncodeSingleBlockSection(buffer, palette[0], biomeEntries[0]);
			return;
		}

		if (palette.size() == 1) {
			buffer.WriteShort(palette[0] == BlockState::Air ? 0 : 4096);
			buffer.WriteShort(0);
			buffer.WriteByte(0);
			buffer.WriteVarInt(palette[0]);
		} else {
			buffer.WriteShort(static_cast<int16_t>(nonAirCount));
			buffer.WriteShort(0);

			int bitsPerEntry = 4;
			while ((1 << bitsPerEntry) < static_cast<int>(palette.size())) bitsPerEntry++;

			buffer.WriteByte(static_cast<uint8_t>(bitsPerEntry));
			buffer.WriteVarInt(static_cast<int32_t>(palette.size()));
			for (const int32_t stateId : palette) buffer.WriteVarInt(stateId);

			const int entriesPerLong = 64 / bitsPerEntry;
			const int longCount = (4096 + entriesPerLong - 1) / entriesPerLong;
			for (int longIndex = 0; longIndex < longCount; longIndex++) {
				int64_t packed = 0;
				const int startEntry = longIndex * entriesPerLong;
				for (int offset = 0; offset < entriesPerLong; offset++) {
					const int entryIndex = startEntry + offset;
					if (entryIndex >= 4096) break;
					packed |= (static_cast<int64_t>(blockIndices[entryIndex]) << (offset * bitsPerEntry));
				}
				buffer.WriteLong(packed);
			}
		}

		BiomeEncoder::EncodeBiomes(buffer, biomeEntries);
	}

}
