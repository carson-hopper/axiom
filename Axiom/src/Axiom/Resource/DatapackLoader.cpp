#include "DatapackLoader.h"

#include "Axiom/Core/Log.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace Axiom {

	void DatapackLoader::Load(const std::filesystem::path& datapackDirectory) {
		m_DatapackDirectory = datapackDirectory;

		AX_CORE_INFO("DatapackLoader: scanning for datapacks in {}", datapackDirectory.string());

		if (!std::filesystem::exists(m_DatapackDirectory)) {
			AX_CORE_TRACE("DatapackLoader: datapacks directory does not exist, creating it");
			std::filesystem::create_directories(m_DatapackDirectory);
			return;
		}

		DiscoverDatapacks();
	}

	void DatapackLoader::Reload() {
		AX_CORE_INFO("DatapackLoader: reloading datapacks");
		m_Datapacks.clear();
		DiscoverDatapacks();
	}

	void DatapackLoader::DiscoverDatapacks() {
		if (!std::filesystem::exists(m_DatapackDirectory)) {
			return;
		}

		for (const auto& entry : std::filesystem::directory_iterator(m_DatapackDirectory)) {
			if (!entry.is_directory()) {
				continue;
			}

			auto packMcmetaPath = entry.path() / "pack.mcmeta";
			if (!std::filesystem::exists(packMcmetaPath)) {
				AX_CORE_WARN("DatapackLoader: skipping {} — no pack.mcmeta",
					entry.path().filename().string());
				continue;
			}

			try {
				std::ifstream file(packMcmetaPath);
				auto json = nlohmann::json::parse(file);

				DatapackInfo info;
				info.name = entry.path().filename().string();
				info.path = entry.path();

				if (json.contains("pack")) {
					auto& pack = json["pack"];
					if (pack.contains("description")) {
						info.description = pack["description"].get<std::string>();
					}
					if (pack.contains("pack_format")) {
						info.packFormat = pack["pack_format"].get<int>();
					}
				}

				m_Datapacks.push_back(std::move(info));
				AX_CORE_INFO("DatapackLoader: discovered datapack '{}'", info.name);
			} catch (const nlohmann::json::exception& exception) {
				AX_CORE_ERROR("DatapackLoader: failed to parse pack.mcmeta in {}: {}",
					entry.path().filename().string(), exception.what());
			}
		}

		AX_CORE_INFO("DatapackLoader: found {} datapacks", m_Datapacks.size());
	}

}
