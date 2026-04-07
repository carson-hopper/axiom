#include "TerrainChunkGenerator.h"

namespace Axiom {

	TerrainChunkGenerator::TerrainChunkGenerator(const uint64_t seed)
		: m_TerrainNoise(seed)
		, m_DetailNoise(seed + 1)
		, m_ContinentNoise(seed + 2)
		, m_ErosionNoise(seed + 3)
		, m_RiverNoise(seed + 10)
		, m_BiomeProvider(seed + 6)
		, m_CaveCarver(seed + 4)
		, m_OreDistributor(seed + 9)
		, m_SurfaceDecorator(seed + 11)
		, m_TreePlacer(seed + 8) {}

	ChunkData TerrainChunkGenerator::Generate(const int32_t chunkX, const int32_t chunkZ) {
		std::array<int, 256> heightmap{};
		std::array<BiomeType, 256> biomeMap{};
		std::array<bool, 256> riverMap{};
		int maximumHeight = MinY;

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int worldX = chunkX * 16 + localX;
				const int worldZ = chunkZ * 16 + localZ;
				const int columnIndex = localZ * 16 + localX;

				const double continentalness = GetContinentalness(worldX, worldZ);
				const double erosion = GetErosion(worldX, worldZ);

				biomeMap[columnIndex] = m_BiomeProvider.SelectBiome(continentalness, erosion, worldX, worldZ);
				riverMap[columnIndex] = IsRiver(worldX, worldZ);
				heightmap[columnIndex] = CalculateHeight(worldX, worldZ, biomeMap[columnIndex]);

				if (riverMap[columnIndex] && heightmap[columnIndex] > SeaLevel) {
					heightmap[columnIndex] = SeaLevel - 1;
				}
				if (heightmap[columnIndex] > maximumHeight) maximumHeight = heightmap[columnIndex];
			}
		}

		std::vector<int32_t> columnBlocks(16 * 16 * WorldHeight, BlockState::Air);
		FillTerrain(chunkX, chunkZ, heightmap, biomeMap, riverMap, columnBlocks);
		m_TreePlacer.PlaceTrees(chunkX, chunkZ, heightmap, biomeMap, columnBlocks);
		PlaceVegetation(chunkX, chunkZ, heightmap, biomeMap, columnBlocks);

		NetworkBuffer sectionData;
		const int32_t centerBiomeId = BiomeProvider::ToRegistryId(biomeMap[128]);
		for (int sectionIndex = 0; sectionIndex < 24; sectionIndex++) {
			EncodeSection(sectionData, MinY + sectionIndex * 16, columnBlocks, centerBiomeId);
		}

		return ChunkData{chunkX, chunkZ, std::move(sectionData.Data()),
			std::max(SeaLevel - MinY + 1, maximumHeight - MinY + 1), centerBiomeId};
	}

	double TerrainChunkGenerator::SpawnY() const {
		const auto biome = m_BiomeProvider.SelectBiome(
			GetContinentalness(0, 0), GetErosion(0, 0), 0, 0);
		return static_cast<double>(std::max(CalculateHeight(0, 0, biome), SeaLevel)) + 1.0;
	}

	double TerrainChunkGenerator::GetContinentalness(const int worldX, const int worldZ) const {
		return m_ContinentNoise.OctaveNoise(worldX * 0.001, worldZ * 0.001, 4, 0.5);
	}

	double TerrainChunkGenerator::GetErosion(const int worldX, const int worldZ) const {
		return m_ErosionNoise.OctaveNoise(worldX * 0.002, worldZ * 0.002, 3, 0.5);
	}

	bool TerrainChunkGenerator::IsRiver(const int worldX, const int worldZ) const {
		return std::abs(m_RiverNoise.OctaveNoise(worldX * 0.004, worldZ * 0.004, 3, 0.5)) < 0.03;
	}

	int TerrainChunkGenerator::CalculateHeight(const int worldX, const int worldZ,
		const BiomeType biome) const {

		const double blockX = static_cast<double>(worldX);
		const double blockZ = static_cast<double>(worldZ);
		const double continentalness = m_ContinentNoise.OctaveNoise(blockX * 0.001, blockZ * 0.001, 4, 0.5);
		const double erosion = m_ErosionNoise.OctaveNoise(blockX * 0.002, blockZ * 0.002, 3, 0.5);
		const double terrain = m_TerrainNoise.OctaveNoise(blockX * 0.008, blockZ * 0.008, 6, 0.5);
		const double detail = m_DetailNoise.OctaveNoise(blockX * 0.03, blockZ * 0.03, 3, 0.4);

		double baseHeight = 64.0;
		const double continentFactor = continentalness * 40.0;
		double terrainAmplitude = terrain * 20.0 * std::max(0.0, 0.5 + continentalness);
		terrainAmplitude *= (1.0 - std::abs(erosion) * 0.5);
		double detailAmplitude = detail * 4.0;

		switch (biome) {
			case BiomeType::Ocean:    baseHeight = 40.0; terrainAmplitude *= 0.3; break;
			case BiomeType::Beach:    baseHeight = 62.0; terrainAmplitude = 0; detailAmplitude *= 0.3; break;
			case BiomeType::Desert: case BiomeType::Savanna: terrainAmplitude *= 0.5; break;
			case BiomeType::Mountain: baseHeight = 90.0; terrainAmplitude *= 2.5; break;
			case BiomeType::Swamp:    baseHeight = 62.0; terrainAmplitude *= 0.2; break;
			default: break;
		}

		return std::clamp(static_cast<int>(baseHeight + continentFactor + terrainAmplitude + detailAmplitude),
			MinY + 1, MaxY - 1);
	}

	void TerrainChunkGenerator::FillTerrain(const int32_t chunkX, const int32_t chunkZ,
		const std::array<int, 256>& heightmap, const std::array<BiomeType, 256>& biomeMap,
		const std::array<bool, 256>& riverMap, std::vector<int32_t>& columnBlocks) const {

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int columnIndex = localZ * 16 + localX;
				const int worldX = chunkX * 16 + localX;
				const int worldZ = chunkZ * 16 + localZ;
				const int surfaceHeight = heightmap[columnIndex];
				const BiomeType biome = biomeMap[columnIndex];
				const int fillHeight = std::max(surfaceHeight, SeaLevel);

				for (int worldY = MinY; worldY <= fillHeight; worldY++) {
					int32_t blockState = GetTerrainBlock(worldY, surfaceHeight, biome, riverMap[columnIndex]);

					if (worldY < surfaceHeight - 1 && worldY > MinY + 5 && blockState != BlockState::Water) {
						if (m_CaveCarver.ShouldCarve(worldX, worldY, worldZ, SeaLevel)) {
							blockState = (worldY <= SeaLevel) ? BlockState::Water : BlockState::Air;
						}
					}
					if (blockState == BlockState::Stone || blockState == BlockState::Deepslate) {
						const int32_t oreBlock = m_OreDistributor.GetOre(
							worldX, worldY, worldZ, blockState == BlockState::Deepslate);
						if (oreBlock != 0) blockState = oreBlock;
					}
					columnBlocks[(worldY - MinY) * 256 + localZ * 16 + localX] = blockState;
				}
			}
		}
	}

	int32_t TerrainChunkGenerator::GetTerrainBlock(const int worldY, const int surfaceHeight,
		const BiomeType biome, const bool isRiver) const {

		if (worldY > surfaceHeight) {
			if (worldY <= SeaLevel) {
				return (biome == BiomeType::SnowyPlains && worldY == SeaLevel)
					? BlockState::PackedIce : BlockState::Water;
			}
			return BlockState::Air;
		}
		if (worldY == surfaceHeight) {
			return isRiver ? BlockState::Sand : m_SurfaceDecorator.GetSurfaceBlock(surfaceHeight, biome);
		}
		if (worldY > surfaceHeight - 4) return m_SurfaceDecorator.GetSubSurfaceBlock(surfaceHeight, biome);
		if (worldY <= MinY + 4) return BlockState::Bedrock;
		if (worldY <= 0) return BlockState::Deepslate;
		return BlockState::Stone;
	}

	void TerrainChunkGenerator::PlaceVegetation(const int32_t chunkX, const int32_t chunkZ,
		const std::array<int, 256>& heightmap, const std::array<BiomeType, 256>& biomeMap,
		std::vector<int32_t>& columnBlocks) const {

		for (int localZ = 0; localZ < 16; localZ++) {
			for (int localX = 0; localX < 16; localX++) {
				const int columnIndex = localZ * 16 + localX;
				const int surfaceHeight = heightmap[columnIndex];
				if (surfaceHeight <= SeaLevel) continue;

				const int surfaceAbsoluteY = surfaceHeight - MinY;
				const int blockIndex = localZ * 16 + localX;
				const int32_t surfaceBlock = columnBlocks[surfaceAbsoluteY * 256 + blockIndex];
				const int32_t aboveBlock = columnBlocks[(surfaceAbsoluteY + 1) * 256 + blockIndex];
				if (aboveBlock != BlockState::Air) continue;

				const bool canDecorate = surfaceBlock == BlockState::GrassBlock
					|| surfaceBlock == BlockState::Podzol || surfaceBlock == BlockState::Stone;
				if (!canDecorate) continue;

				const int32_t vegetation = m_SurfaceDecorator.GetVegetation(
					chunkX * 16 + localX, chunkZ * 16 + localZ, biomeMap[columnIndex]);
				if (vegetation != BlockState::Air) {
					columnBlocks[(surfaceAbsoluteY + 1) * 256 + blockIndex] = vegetation;
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
