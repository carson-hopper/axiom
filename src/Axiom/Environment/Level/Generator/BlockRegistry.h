#pragma once

#include "Axiom/Core/Base.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Maps block and biome names to their numeric IDs.
	 * Loaded from extractor JSON data at startup.
	 */
	class BlockRegistry {
	public:
		void LoadFromExtractorData(const std::string& dataDirectory) {
			LoadBlocks(dataDirectory + "/blocks.json");
			LoadBiomes(dataDirectory + "/synced_registries.json");
		}

		int32_t GetBlockStateId(const std::string& blockName) const {
			const auto iterator = m_BlockNameToStateId.find(blockName);
			if (iterator != m_BlockNameToStateId.end()) {
				return iterator->second;
			}
			return 0; // Air for unknown blocks
		}

		int32_t GetBiomeId(const std::string& biomeName) const {
			const auto iterator = m_BiomeNameToId.find(biomeName);
			if (iterator != m_BiomeNameToId.end()) {
				return iterator->second;
			}
			return 0; // First biome for unknown
		}

		int BlockCount() const { return static_cast<int>(m_BlockNameToStateId.size()); }
		int BiomeCount() const { return static_cast<int>(m_BiomeNameToId.size()); }

	private:
		void LoadBlocks(const std::string& path) {
			std::ifstream file(path);
			if (!file.good()) return;

			try {
				auto json = nlohmann::json::parse(file);
				const auto& blocks = json["blocks"];

				for (const auto& block : blocks) {
					const std::string name = "minecraft:" + block["name"].get<std::string>();
					int32_t defaultStateId = 0;

					if (block.contains("default_state_id")) {
						defaultStateId = block["default_state_id"].get<int32_t>();
					} else if (block.contains("states") && !block["states"].empty()) {
						defaultStateId = block["states"][0]["id"].get<int32_t>();
					}

					m_BlockNameToStateId[name] = defaultStateId;

					// Also map states with properties for blocks that have multiple states
					if (block.contains("states")) {
						for (const auto& state : block["states"]) {
							// Store the first state as the block's default
							// Individual states would need property matching for full accuracy
						}
					}
				}
			} catch (const nlohmann::json::exception&) {
				// Silent fail — partial data is better than none
			}
		}

		void LoadBiomes(const std::string& path) {
			std::ifstream file(path);
			if (!file.good()) return;

			try {
				auto json = nlohmann::json::parse(file);
				if (!json.contains("worldgen/biome")) return;

				int32_t index = 0;
				for (auto& [name, data] : json["worldgen/biome"].items()) {
					m_BiomeNameToId["minecraft:" + name] = index++;
				}
			} catch (const nlohmann::json::exception&) {}
		}

		std::unordered_map<std::string, int32_t> m_BlockNameToStateId;
		std::unordered_map<std::string, int32_t> m_BiomeNameToId;
	};

}
