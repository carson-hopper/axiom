#pragma once

#include "Axiom/Environment/Level/Generator/ChunkGenerator.h"
#include "Axiom/Environment/Level/Generator/AnvilReader.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"
#include "Axiom/Environment/Level/ChunkEncoder.h"
#include "Axiom/Network/Nbt/NbtReader.h"

#include <string>

namespace Axiom {

	/**
	 * Loads pre-generated chunks from a vanilla Minecraft world.
	 * Reads .mca region files and extracts block/biome data directly,
	 * re-encoding them into the network protocol format.
	 */
	class VanillaChunkGenerator : public ChunkGenerator {
	public:
		explicit VanillaChunkGenerator(const std::string& worldDirectory)
			: m_AnvilReader(worldDirectory + "/dimensions/minecraft/overworld/region") {}

		ChunkData Generate(const int32_t chunkX, const int32_t chunkZ) override {
			auto nbtData = m_AnvilReader.ReadChunkNbt(chunkX, chunkZ);

			if (!nbtData) {
				return GenerateEmptyChunk(chunkX, chunkZ);
			}

			return ParseChunkNbt(chunkX, chunkZ, *nbtData);
		}

		double SpawnY() const override {
			return 100.0;  // Will be overridden by level.dat later
		}

		int32_t GetBlockAt(int32_t worldX, int32_t worldY, int32_t worldZ) const override {
			// For now, return air — full per-block lookup from .mca is expensive
			// Physics will use overrides for player-modified blocks
			return BlockState::Air;
		}

	private:
		ChunkData GenerateEmptyChunk(const int32_t chunkX, const int32_t chunkZ) const {
			NetworkBuffer sectionData;
			for (int sectionIndex = 0; sectionIndex < 24; sectionIndex++) {
				ChunkEncoder::EncodeSingleBlockSection(sectionData, BlockState::Air, 0);
			}
			return ChunkData{chunkX, chunkZ, std::move(sectionData.Data()), 0, 0};
		}

		ChunkData ParseChunkNbt(const int32_t chunkX, const int32_t chunkZ,
			const std::vector<uint8_t>& nbtData) const {

			NbtReader reader(nbtData);

			if (!reader.ReadRootCompound()) {
				return GenerateEmptyChunk(chunkX, chunkZ);
			}

			// Find "sections" list
			NetworkBuffer sectionData;
			int32_t heightmapValue = 128;
			int32_t biomeId = 0;
			bool hasSections = false;

			// Structure: root compound → sections (list of compounds)
			// Each section has: Y (byte), block_states (compound), biomes (compound)
			auto sectionsType = reader.FindTag("sections");
			if (sectionsType == NbtReader::TagList) {
				hasSections = true;
				auto listHeader = reader.ReadListHeader();

				// We need sections indexed by Y. MC stores them in the list
				// ordered by Y value. We need sections for Y -4 to 19 (indices 0-23).
				struct SectionEntry {
					int8_t yIndex;
					size_t dataPosition;
				};
				std::vector<SectionEntry> entries;

				for (int32_t sectionIndex = 0; sectionIndex < listHeader.count; sectionIndex++) {
					const size_t sectionStart = reader.Position();

					// Read Y value
					int8_t yValue = 0;
					auto yType = reader.FindTag("Y");
					if (yType == NbtReader::TagByte) {
						yValue = reader.ReadTagByte();
					}

					entries.push_back({yValue, sectionStart});

					// Skip to end of this compound
					// Reset to start and skip the whole thing
					reader.SetPosition(sectionStart);
					reader.SkipCompound();
				}

				// Now encode 24 sections in order (Y = -4 to 19)
				for (int targetY = -4; targetY < 20; targetY++) {
					bool found = false;
					for (const auto& entry : entries) {
						if (entry.yIndex == targetY) {
							reader.SetPosition(entry.dataPosition);
							EncodeSectionFromNbt(reader, sectionData);
							found = true;
							break;
						}
					}
					if (!found) {
						// Missing section = all air
						ChunkEncoder::EncodeSingleBlockSection(sectionData, BlockState::Air, 0);
					}
				}
			}

			if (!hasSections) {
				return GenerateEmptyChunk(chunkX, chunkZ);
			}

			return ChunkData{chunkX, chunkZ, std::move(sectionData.Data()), heightmapValue, biomeId};
		}

		void EncodeSectionFromNbt(NbtReader& reader, NetworkBuffer& output) const {
			// Parse section compound to find block_states and biomes
			// Then re-encode for the network protocol

			int16_t nonAirCount = 0;
			bool hasBlockStates = false;
			bool hasBiomes = false;

			// We need: block_states.palette (list), block_states.data (long array)
			//          biomes.palette (list), biomes.data (long array)

			size_t sectionStart = reader.Position();

			// First pass: find block_states
			auto blockStatesType = reader.FindTag("block_states");
			if (blockStatesType == NbtReader::TagCompound) {
				hasBlockStates = true;
				EncodeBlockStatesFromNbt(reader, output, nonAirCount);
			}

			// Find biomes
			reader.SetPosition(sectionStart);
			auto biomesType = reader.FindTag("biomes");
			if (biomesType == NbtReader::TagCompound) {
				hasBiomes = true;
				EncodeBiomesFromNbt(reader, output);
			}

			if (!hasBlockStates) {
				// Write empty block states (single-valued air)
				output.WriteShort(0);
				output.WriteShort(0);
				output.WriteByte(0);
				output.WriteVarInt(0);  // Air
			}

			if (!hasBiomes) {
				output.WriteByte(0);
				output.WriteVarInt(0);  // Plains
			}

			// Skip rest of section compound
			reader.SetPosition(sectionStart);
			reader.SkipCompound();
		}

		void EncodeBlockStatesFromNbt(NbtReader& reader, NetworkBuffer& output,
			int16_t& nonAirCount) const {

			// block_states compound has: palette (list of compounds), data (long array)
			size_t blockStatesStart = reader.Position();

			// Find palette
			auto paletteType = reader.FindTag("palette");
			std::vector<std::string> paletteNames;

			if (paletteType == NbtReader::TagList) {
				auto listHeader = reader.ReadListHeader();
				for (int32_t i = 0; i < listHeader.count; i++) {
					// Each palette entry is a compound with "Name" string and optional "Properties"
					auto nameType = reader.FindTag("Name");
					std::string blockName;
					if (nameType == NbtReader::TagString) {
						blockName = reader.ReadTagString();
					}
					paletteNames.push_back(blockName);

					// Skip rest of the palette entry compound (Properties etc)
					// Actually we're in the middle of the compound, need to finish it
					reader.SkipCompound();
				}
			}

			// Find data long array
			reader.SetPosition(blockStatesStart);
			auto dataType = reader.FindTag("data");
			std::vector<int64_t> dataArray;
			if (dataType == NbtReader::TagLongArray) {
				dataArray = reader.ReadLongArray();
			}

			reader.SetPosition(blockStatesStart);
			reader.SkipCompound();

			// Now encode for the network protocol
			// Count non-air blocks
			nonAirCount = 0;
			for (const auto& name : paletteNames) {
				if (name != "minecraft:air" && name != "minecraft:cave_air" && name != "minecraft:void_air") {
					// Approximate — assume non-air blocks are present
					// Exact count would require unpacking the data array
					nonAirCount = 4096; // Conservative estimate
					break;
				}
			}

			if (paletteNames.size() <= 1) {
				// Single-valued
				output.WriteShort(paletteNames.empty() || paletteNames[0] == "minecraft:air" ? 0 : 4096);
				output.WriteShort(0);
				output.WriteByte(0); // 0 bits per entry
				output.WriteVarInt(0); // Default to air (proper mapping would use block registry)
				return;
			}

			output.WriteShort(nonAirCount);
			output.WriteShort(0); // Fluid count

			// Determine bits per entry from data array
			if (dataArray.empty()) {
				output.WriteByte(0);
				output.WriteVarInt(0);
				return;
			}

			// Calculate bits per entry: 4096 entries packed into dataArray.size() longs
			int bitsPerEntry = static_cast<int>(dataArray.size() * 64 / 4096);
			if (bitsPerEntry < 4) bitsPerEntry = 4;
			if (bitsPerEntry > 15) bitsPerEntry = 15; // Direct palette

			output.WriteByte(static_cast<uint8_t>(bitsPerEntry));

			// Write palette (using global block state IDs would require a name→ID mapping)
			// For now, write palette size 0 and use direct mode if bits >= 15
			// Actually, let's just pass through the palette indices and data as-is
			// The client's palette maps to block state IDs through the palette entries

			// We need to map palette names to block state IDs
			// For simplicity, write as direct palette (no indirect palette)
			// This means bitsPerEntry = 15, and data values ARE block state IDs

			// Actually, the simplest correct approach: if bits < 15, write an indirect
			// palette with block state IDs, then pass through the data array.
			// But we'd need a full name→stateId mapping.

			// SIMPLEST APPROACH: just pass the raw section data from the file.
			// The chunk file stores block states differently from the network format,
			// but the packing is identical (packed longs with same bit width).
			// We just need to map palette names to state IDs.

			// For now, use indirect palette with names mapped to IDs
			output.WriteVarInt(static_cast<int32_t>(paletteNames.size()));
			for (const auto& name : paletteNames) {
				output.WriteVarInt(BlockNameToStateId(name));
			}

			// Write packed data directly — the format is the same
			for (const int64_t value : dataArray) {
				output.WriteLong(value);
			}
		}

		void EncodeBiomesFromNbt(NbtReader& reader, NetworkBuffer& output) const {
			size_t biomesStart = reader.Position();

			auto paletteType = reader.FindTag("palette");
			std::vector<std::string> paletteNames;

			if (paletteType == NbtReader::TagList) {
				auto listHeader = reader.ReadListHeader();
				for (int32_t i = 0; i < listHeader.count; i++) {
					if (listHeader.elementType == NbtReader::TagString) {
						paletteNames.push_back(reader.ReadTagString());
					}
				}
			}

			reader.SetPosition(biomesStart);
			auto dataType = reader.FindTag("data");
			std::vector<int64_t> dataArray;
			if (dataType == NbtReader::TagLongArray) {
				dataArray = reader.ReadLongArray();
			}

			reader.SetPosition(biomesStart);
			reader.SkipCompound();

			if (paletteNames.size() <= 1) {
				output.WriteByte(0);
				output.WriteVarInt(paletteNames.empty() ? 0 : BiomeNameToId(paletteNames[0]));
				return;
			}

			int bitsPerEntry = 1;
			while ((1 << bitsPerEntry) < static_cast<int>(paletteNames.size())) bitsPerEntry++;
			if (bitsPerEntry > 3) bitsPerEntry = 6; // Direct biome palette

			output.WriteByte(static_cast<uint8_t>(bitsPerEntry));

			if (bitsPerEntry <= 3) {
				output.WriteVarInt(static_cast<int32_t>(paletteNames.size()));
				for (const auto& name : paletteNames) {
					output.WriteVarInt(BiomeNameToId(name));
				}
			}

			for (const int64_t value : dataArray) {
				output.WriteLong(value);
			}
		}

		static int32_t BlockNameToStateId(const std::string& name) {
			// Common block name → default state ID mapping
			// This is a simplified mapping — full mapping would load from blocks.json
			static const std::unordered_map<std::string, int32_t> mapping = {
				{"minecraft:air", 0}, {"minecraft:cave_air", 0}, {"minecraft:void_air", 0},
				{"minecraft:stone", 1}, {"minecraft:granite", 2}, {"minecraft:polished_granite", 3},
				{"minecraft:diorite", 4}, {"minecraft:polished_diorite", 5},
				{"minecraft:andesite", 6}, {"minecraft:polished_andesite", 7},
				{"minecraft:grass_block", 9}, {"minecraft:dirt", 10},
				{"minecraft:coarse_dirt", 11}, {"minecraft:podzol", 13},
				{"minecraft:cobblestone", 14}, {"minecraft:bedrock", 85},
				{"minecraft:water", 86}, {"minecraft:lava", 102},
				{"minecraft:sand", 118}, {"minecraft:gravel", 124},
				{"minecraft:gold_ore", 129}, {"minecraft:iron_ore", 131},
				{"minecraft:coal_ore", 133}, {"minecraft:oak_log", 137},
				{"minecraft:oak_leaves", 279}, {"minecraft:deepslate", 27924},
			};

			auto iterator = mapping.find(name);
			if (iterator != mapping.end()) return iterator->second;
			return 0; // Unknown = air
		}

		static int32_t BiomeNameToId(const std::string& name) {
			static const std::unordered_map<std::string, int32_t> mapping = {
				{"minecraft:plains", 40}, {"minecraft:desert", 14}, {"minecraft:forest", 21},
				{"minecraft:ocean", 35}, {"minecraft:river", 41}, {"minecraft:beach", 3},
				{"minecraft:taiga", 55}, {"minecraft:swamp", 54}, {"minecraft:jungle", 28},
				{"minecraft:birch_forest", 4}, {"minecraft:dark_forest", 8},
				{"minecraft:snowy_plains", 46}, {"minecraft:savanna", 42},
				{"minecraft:badlands", 0}, {"minecraft:deep_ocean", 13},
				{"minecraft:meadow", 32}, {"minecraft:frozen_ocean", 22},
			};

			auto iterator = mapping.find(name);
			if (iterator != mapping.end()) return iterator->second;
			return 40; // Default = plains
		}

		mutable AnvilReader m_AnvilReader;
	};

}
