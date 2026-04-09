#include "LootTableManager.h"

#include "Axiom/Core/Log.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace Axiom {

	void LootTableManager::Load(const std::filesystem::path& lootTableDirectory) {
		m_LootTableDirectory = lootTableDirectory;

		AX_CORE_INFO("LootTableManager: loading loot tables from {}", lootTableDirectory.string());

		if (!std::filesystem::exists(m_LootTableDirectory)) {
			AX_CORE_TRACE("LootTableManager: loot tables directory does not exist");
			return;
		}

		LoadDirectory(m_LootTableDirectory);

		AX_CORE_INFO("LootTableManager: loaded {} loot tables", m_LootTables.size());
	}

	void LootTableManager::Reload() {
		AX_CORE_INFO("LootTableManager: reloading loot tables");
		m_LootTables.clear();

		if (std::filesystem::exists(m_LootTableDirectory)) {
			LoadDirectory(m_LootTableDirectory);
		}

		AX_CORE_INFO("LootTableManager: reloaded {} loot tables", m_LootTables.size());
	}

	void LootTableManager::LoadDirectory(const std::filesystem::path& directory) {
		for (const auto& entry : std::filesystem::directory_iterator(directory)) {
			if (entry.is_directory()) {
				LoadDirectory(entry.path());
				continue;
			}

			if (entry.path().extension() != ".json") {
				continue;
			}

			try {
				std::ifstream file(entry.path());
				auto json = nlohmann::json::parse(file);

				LootTableEntry lootTable;
				lootTable.name = entry.path().stem().string();
				lootTable.sourcePath = entry.path();

				if (json.contains("type")) {
					lootTable.type = json["type"].get<std::string>();
				}

				// Stub: real evaluation would parse pools,
				// entries, conditions, and functions

				m_LootTables.push_back(std::move(lootTable));
				AX_CORE_TRACE("LootTableManager: loaded loot table '{}'", lootTable.name);
			} catch (const nlohmann::json::exception& exception) {
				AX_CORE_ERROR("LootTableManager: failed to parse {}: {}",
					entry.path().filename().string(), exception.what());
			}
		}
	}

}
