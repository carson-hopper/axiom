#pragma once

#include "Axiom/Core/Base.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Represents a loaded loot table with
	 * its identifier and source path.
	 */
	struct LootTableEntry {
		std::string name;
		std::string type;
		std::filesystem::path sourcePath;
	};

	/**
	 * Loads loot table JSON definitions.
	 * Loot tables define item drops from
	 * blocks, entities, chests, fishing,
	 * and other game mechanics.
	 */
	class LootTableManager {
	public:
		LootTableManager() = default;
		~LootTableManager() = default;

		/**
		 * Load all loot tables from
		 * the given directory.
		 */
		void Load(const std::filesystem::path& lootTableDirectory);

		/**
		 * Reload all loot tables from
		 * the last loaded directory.
		 */
		void Reload();

		const std::vector<LootTableEntry>& LootTables() const { return m_LootTables; }
		size_t Count() const { return m_LootTables.size(); }

	private:
		void LoadDirectory(const std::filesystem::path& directory);

		std::filesystem::path m_LootTableDirectory;
		std::vector<LootTableEntry> m_LootTables;
	};

}
