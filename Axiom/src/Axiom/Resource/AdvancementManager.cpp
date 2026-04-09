#include "AdvancementManager.h"

#include "Axiom/Core/Log.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace Axiom {

	void AdvancementManager::Load(const std::filesystem::path& advancementDirectory) {
		m_AdvancementDirectory = advancementDirectory;

		AX_CORE_INFO("AdvancementManager: loading advancements from {}", advancementDirectory.string());

		if (!std::filesystem::exists(m_AdvancementDirectory)) {
			AX_CORE_TRACE("AdvancementManager: advancements directory does not exist");
			return;
		}

		LoadDirectory(m_AdvancementDirectory);

		AX_CORE_INFO("AdvancementManager: loaded {} advancements", m_Advancements.size());
	}

	void AdvancementManager::Reload() {
		AX_CORE_INFO("AdvancementManager: reloading advancements");
		m_Advancements.clear();

		if (std::filesystem::exists(m_AdvancementDirectory)) {
			LoadDirectory(m_AdvancementDirectory);
		}

		AX_CORE_INFO("AdvancementManager: reloaded {} advancements", m_Advancements.size());
	}

	void AdvancementManager::LoadDirectory(const std::filesystem::path& directory) {
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

				AdvancementEntry advancement;
				advancement.name = entry.path().stem().string();
				advancement.sourcePath = entry.path();

				if (json.contains("parent")) {
					advancement.parent = json["parent"].get<std::string>();
				}

				m_Advancements.push_back(std::move(advancement));
				AX_CORE_TRACE("AdvancementManager: loaded advancement '{}'", advancement.name);
			} catch (const nlohmann::json::exception& exception) {
				AX_CORE_ERROR("AdvancementManager: failed to parse {}: {}",
					entry.path().filename().string(), exception.what());
			}
		}
	}

}
