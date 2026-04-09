#pragma once

#include "Axiom/Core/Base.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Represents a loaded recipe with
	 * its type and raw JSON data.
	 */
	struct RecipeEntry {
		std::string name;
		std::string type;
		std::string group;
		std::filesystem::path sourcePath;
	};

	/**
	 * Loads and validates crafting recipes
	 * from JSON files. Supports shaped,
	 * shapeless, smelting, and other
	 * recipe types.
	 */
	class RecipeManager {
	public:
		RecipeManager() = default;
		~RecipeManager() = default;

		/**
		 * Load all recipes from the
		 * given directory.
		 */
		void Load(const std::filesystem::path& recipeDirectory);

		/**
		 * Reload all recipes from
		 * the last loaded directory.
		 */
		void Reload();

		const std::vector<RecipeEntry>& Recipes() const { return m_Recipes; }
		size_t Count() const { return m_Recipes.size(); }

	private:
		void LoadDirectory(const std::filesystem::path& directory);

		std::filesystem::path m_RecipeDirectory;
		std::vector<RecipeEntry> m_Recipes;
	};

}
