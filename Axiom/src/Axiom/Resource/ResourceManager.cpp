#include "ResourceManager.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Resource/DatapackLoader.h"
#include "Axiom/Resource/TagLoader.h"
#include "Axiom/Resource/RecipeManager.h"
#include "Axiom/Resource/LootTableManager.h"
#include "Axiom/Resource/AdvancementManager.h"

namespace Axiom {

	ResourceManager::ResourceManager()
		: m_DatapackLoader(CreateScope<DatapackLoader>())
		, m_TagLoader(CreateScope<TagLoader>())
		, m_RecipeManager(CreateScope<RecipeManager>())
		, m_LootTableManager(CreateScope<LootTableManager>())
		, m_AdvancementManager(CreateScope<AdvancementManager>()) {
	}

	ResourceManager::~ResourceManager() = default;

	void ResourceManager::Load(const std::filesystem::path& dataDirectory) {
		m_DataDirectory = dataDirectory;

		AX_CORE_INFO("ResourceManager: loading resources from {}", dataDirectory.string());

		m_DatapackLoader->Load(dataDirectory / "datapacks");
		m_TagLoader->Load(dataDirectory / "tags");
		m_RecipeManager->Load(dataDirectory / "recipes");
		m_LootTableManager->Load(dataDirectory / "loot_tables");
		m_AdvancementManager->Load(dataDirectory / "advancements");

		AX_CORE_INFO("ResourceManager: all resources loaded");
	}

	void ResourceManager::Reload() {
		AX_CORE_INFO("ResourceManager: reloading all resources");

		m_DatapackLoader->Reload();
		m_TagLoader->Reload();
		m_RecipeManager->Reload();
		m_LootTableManager->Reload();
		m_AdvancementManager->Reload();

		AX_CORE_INFO("ResourceManager: reload complete");
	}

}
