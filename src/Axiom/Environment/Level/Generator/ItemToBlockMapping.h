#pragma once

#include "Axiom/Core/Base.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Maps item IDs to their corresponding block state IDs.
	 * Used to determine what block to place when a player right-clicks.
	 */
	class ItemToBlockMapping {
	public:
		void LoadFromExtractorData(const std::string& dataDirectory) {
			std::ifstream file(dataDirectory + "/blocks.json");
			if (!file.good()) return;

			try {
				auto json = nlohmann::json::parse(file);
				for (const auto& block : json["blocks"]) {
					const int32_t itemId = block.value("item_id", 0);
					if (itemId <= 0) continue;

					int32_t defaultStateId = 0;
					if (block.contains("default_state_id")) {
						defaultStateId = block["default_state_id"].get<int32_t>();
					} else if (block.contains("states") && !block["states"].empty()) {
						defaultStateId = block["states"][0]["id"].get<int32_t>();
					}

					m_ItemToBlockState[itemId] = defaultStateId;
				}
			} catch (const nlohmann::json::exception&) {}
		}

		/**
		 * Get the block state ID for placing a block from an item.
		 * Returns 0 (air) if the item isn't a block.
		 */
		int32_t GetBlockState(const int32_t itemId) const {
			const auto iterator = m_ItemToBlockState.find(itemId);
			if (iterator != m_ItemToBlockState.end()) {
				return iterator->second;
			}
			return 0;
		}

		bool IsBlockItem(const int32_t itemId) const {
			return m_ItemToBlockState.contains(itemId);
		}

	private:
		std::unordered_map<int32_t, int32_t> m_ItemToBlockState;
	};

}
