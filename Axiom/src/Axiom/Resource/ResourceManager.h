#pragma once

#include "Axiom/Core/Base.h"

#include <filesystem>
#include <string>
#include <vector>

namespace Axiom {

	class DatapackLoader;
	class TagLoader;
	class RecipeManager;
	class LootTableManager;
	class AdvancementManager;

	/**
	 * Central manager for all server resources.
	 * Coordinates loading and reloading of
	 * datapacks, tags, recipes, loot tables,
	 * and advancements.
	 */
	class ResourceManager {
	public:
		ResourceManager();
		~ResourceManager();

		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;

		/**
		 * Load all resources from the
		 * given root directory.
		 */
		void Load(const std::filesystem::path& dataDirectory);

		/**
		 * Reload all resources, typically
		 * triggered by the /reload command
		 * or datapack changes.
		 */
		void Reload();

		DatapackLoader& Datapacks() const { return *m_DatapackLoader; }
		TagLoader& Tags() const { return *m_TagLoader; }
		RecipeManager& Recipes() const { return *m_RecipeManager; }
		LootTableManager& LootTables() const { return *m_LootTableManager; }
		AdvancementManager& Advancements() const { return *m_AdvancementManager; }

	private:
		std::filesystem::path m_DataDirectory;
		Scope<DatapackLoader> m_DatapackLoader;
		Scope<TagLoader> m_TagLoader;
		Scope<RecipeManager> m_RecipeManager;
		Scope<LootTableManager> m_LootTableManager;
		Scope<AdvancementManager> m_AdvancementManager;
	};

}
