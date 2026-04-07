#include "TerrainChunkGenerator.h"

namespace Axiom {

	TerrainChunkGenerator::TerrainChunkGenerator(const uint64_t seed)
		: m_DensityProvider(seed)
		, m_RiverNoise(seed + 10)
		, m_BiomeProvider(seed + 20)
		, m_CaveCarver(seed + 30)
		, m_OreDistributor(seed + 40)
		, m_SurfaceDecorator(seed + 50)
		, m_TreePlacer(seed + 60) {}

	ChunkData TerrainChunkGenerator::Generate(const int32_t chunkX, const int32_t chunkZ) {
		std::array<BiomeType, 256> biomeMap{};
		std::array<bool, 256> riverMap{};

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int worldX = chunkX * 16 + localX;
				const int worldZ = chunkZ * 16 + localZ;
				const int columnIndex = localZ * 16 + localX;

				const double continentalness = m_DensityProvider.GetContinentalness(worldX, worldZ);
				const double erosion = m_DensityProvider.GetErosion(worldX, worldZ);

				biomeMap[columnIndex] = m_BiomeProvider.SelectBiome(continentalness, erosion, worldX, worldZ);
				riverMap[columnIndex] = IsRiver(worldX, worldZ);
			}
		}

		std::vector<int32_t> columnBlocks(16 * 16 * WorldHeight, BlockState::Air);
		std::array<int, 256> surfaceHeightmap{};

		FillTerrainDensity(chunkX, chunkZ, surfaceHeightmap, biomeMap, riverMap, columnBlocks);
		ApplySurfaceBlocks(chunkX, chunkZ, surfaceHeightmap, biomeMap, riverMap, columnBlocks);
		m_TreePlacer.PlaceTrees(chunkX, chunkZ, surfaceHeightmap, biomeMap, columnBlocks);
		PlaceVegetation(chunkX, chunkZ, surfaceHeightmap, biomeMap, columnBlocks);

		NetworkBuffer sectionData;
		const int32_t centerBiomeId = BiomeProvider::ToRegistryId(biomeMap[128]);
		for (int sectionIndex = 0; sectionIndex < 24; sectionIndex++) {
			EncodeSection(sectionData, MinY + sectionIndex * 16, columnBlocks, centerBiomeId);
		}

		int maximumHeight = MinY;
		for (const int height : surfaceHeightmap) {
			if (height > maximumHeight) maximumHeight = height;
		}

		return ChunkData{chunkX, chunkZ, std::move(sectionData.Data()),
			std::max(SeaLevel - MinY + 1, maximumHeight - MinY + 1), centerBiomeId};
	}

	double TerrainChunkGenerator::SpawnY() const {
		return static_cast<double>(
			std::max(m_DensityProvider.ComputeSurfaceHeight(0, 0), SeaLevel)) + 1.0;
	}

	bool TerrainChunkGenerator::IsRiver(const int worldX, const int worldZ) const {
		return std::abs(m_RiverNoise.OctaveNoise(worldX * 0.004, worldZ * 0.004, 3, 0.5)) < 0.03;
	}

	void TerrainChunkGenerator::FillTerrainDensity(const int32_t chunkX, const int32_t chunkZ,
		std::array<int, 256>& surfaceHeightmap,
		const std::array<BiomeType, 256>& biomeMap,
		const std::array<bool, 256>& riverMap,
		std::vector<int32_t>& columnBlocks) const {

		surfaceHeightmap.fill(MinY);

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int worldX = chunkX * 16 + localX;
				const int worldZ = chunkZ * 16 + localZ;
				const int columnIndex = localZ * 16 + localX;
				const bool isRiver = riverMap[columnIndex];

				const int approximateHeight = m_DensityProvider.ComputeSurfaceHeight(worldX, worldZ);
				const int scanMaxY = std::min(MaxY, approximateHeight + 30);

				int surfaceHeight = MinY;

				for (int worldY = MinY; worldY <= scanMaxY; worldY++) {
					const double density = m_DensityProvider.SampleDensity(worldX, worldY, worldZ);

					const bool isRiverCarved = isRiver
						&& worldY >= SeaLevel - 2 && worldY <= SeaLevel + 2
						&& density > 0 && biomeMap[columnIndex] != BiomeType::Ocean;

					if (density > 0 && !isRiverCarved) {
						if (worldY <= MinY + 4) {
							columnBlocks[(worldY - MinY) * 256 + columnIndex] = BlockState::Bedrock;
						} else if (worldY <= 0) {
							columnBlocks[(worldY - MinY) * 256 + columnIndex] = BlockState::Deepslate;
						} else {
							columnBlocks[(worldY - MinY) * 256 + columnIndex] = BlockState::Stone;
						}
						surfaceHeight = worldY;
					} else if (worldY <= SeaLevel) {
						columnBlocks[(worldY - MinY) * 256 + columnIndex] = BlockState::Water;
					}
				}

				surfaceHeightmap[columnIndex] = surfaceHeight;

				// Cave carving
				for (int worldY = MinY + 5; worldY < surfaceHeight - 1; worldY++) {
					const int absoluteY = worldY - MinY;
					const int32_t currentBlock = columnBlocks[absoluteY * 256 + columnIndex];
					if (currentBlock == BlockState::Stone || currentBlock == BlockState::Deepslate) {
						if (m_CaveCarver.ShouldCarve(worldX, worldY, worldZ, SeaLevel)) {
							columnBlocks[absoluteY * 256 + columnIndex] =
								(worldY <= SeaLevel) ? BlockState::Water : BlockState::Air;
						}
					}
				}

				// Ore placement
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

	void TerrainChunkGenerator::ApplySurfaceBlocks(const int32_t chunkX, const int32_t chunkZ,
		const std::array<int, 256>& surfaceHeightmap,
		const std::array<BiomeType, 256>& biomeMap,
		const std::array<bool, 256>& riverMap,
		std::vector<int32_t>& columnBlocks) const {

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int columnIndex = localZ * 16 + localX;
				const int surfaceHeight = surfaceHeightmap[columnIndex];
				if (surfaceHeight <= MinY) continue;

				const BiomeType biome = biomeMap[columnIndex];
				const bool isRiver = riverMap[columnIndex];
				const int surfaceAbsoluteY = surfaceHeight - MinY;
				const int32_t surfaceBlock = columnBlocks[surfaceAbsoluteY * 256 + columnIndex];

				if (surfaceBlock == BlockState::Air || surfaceBlock == BlockState::Water) continue;

				const int32_t newSurface = isRiver
					? BlockState::Sand
					: m_SurfaceDecorator.GetSurfaceBlock(surfaceHeight, biome);
				columnBlocks[surfaceAbsoluteY * 256 + columnIndex] = newSurface;

				const int32_t subSurfaceBlock = m_SurfaceDecorator.GetSubSurfaceBlock(surfaceHeight, biome);
				for (int depth = 1; depth <= 3; depth++) {
					const int belowY = surfaceHeight - depth;
					if (belowY < MinY) break;
					const int32_t existingBlock = columnBlocks[(belowY - MinY) * 256 + columnIndex];
					if (existingBlock == BlockState::Stone || existingBlock == BlockState::Deepslate) {
						columnBlocks[(belowY - MinY) * 256 + columnIndex] = subSurfaceBlock;
					}
				}

				if (biome == BiomeType::SnowyPlains && surfaceHeight < SeaLevel) {
					const int seaAbsoluteY = SeaLevel - MinY;
					if (columnBlocks[seaAbsoluteY * 256 + columnIndex] == BlockState::Water) {
						columnBlocks[seaAbsoluteY * 256 + columnIndex] = BlockState::PackedIce;
					}
				}
			}
		}
	}

	void TerrainChunkGenerator::PlaceVegetation(const int32_t chunkX, const int32_t chunkZ,
		const std::array<int, 256>& surfaceHeightmap,
		const std::array<BiomeType, 256>& biomeMap,
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

				const int32_t vegetation = m_SurfaceDecorator.GetVegetation(
					chunkX * 16 + localX, chunkZ * 16 + localZ, biomeMap[columnIndex]);
				if (vegetation != BlockState::Air) {
					columnBlocks[aboveAbsoluteY * 256 + columnIndex] = vegetation;
				}
			}
		}
	}

	void TerrainChunkGenerator::EncodeSection(NetworkBuffer& buffer, const int sectionMinY,
		const std::vector<int32_t>& columnBlocks, const int32_t biomeId) {

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

		if (palette.size() == 1) {
			ChunkEncoder::EncodeSingleBlockSection(buffer, palette[0], biomeId);
			return;
		}

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

		buffer.WriteByte(0);
		buffer.WriteVarInt(biomeId);
	}

}
