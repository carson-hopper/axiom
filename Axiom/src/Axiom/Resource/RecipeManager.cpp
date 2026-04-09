#include "RecipeManager.h"

#include "Axiom/Core/Log.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace Axiom {

	void RecipeManager::Load(const std::filesystem::path& recipeDirectory) {
		m_RecipeDirectory = recipeDirectory;

		AX_CORE_INFO("RecipeManager: loading recipes from {}", recipeDirectory.string());

		if (!std::filesystem::exists(m_RecipeDirectory)) {
			AX_CORE_TRACE("RecipeManager: recipes directory does not exist");
			return;
		}

		LoadDirectory(m_RecipeDirectory);

		AX_CORE_INFO("RecipeManager: loaded {} recipes", m_Recipes.size());
	}

	void RecipeManager::Reload() {
		AX_CORE_INFO("RecipeManager: reloading recipes");
		m_Recipes.clear();

		if (std::filesystem::exists(m_RecipeDirectory)) {
			LoadDirectory(m_RecipeDirectory);
		}

		AX_CORE_INFO("RecipeManager: reloaded {} recipes", m_Recipes.size());
	}

	void RecipeManager::LoadDirectory(const std::filesystem::path& directory) {
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

				RecipeEntry recipe;
				recipe.name = entry.path().stem().string();
				recipe.sourcePath = entry.path();

				if (json.contains("type")) {
					recipe.type = json["type"].get<std::string>();
				}
				if (json.contains("group")) {
					recipe.group = json["group"].get<std::string>();
				}

				// Stub: real validation would check ingredients,
				// result items, pattern validity, etc.

				m_Recipes.push_back(std::move(recipe));
				AX_CORE_TRACE("RecipeManager: loaded recipe '{}' (type: {})",
					recipe.name, recipe.type);
			} catch (const nlohmann::json::exception& exception) {
				AX_CORE_ERROR("RecipeManager: failed to parse {}: {}",
					entry.path().filename().string(), exception.what());
			}
		}
	}

}
