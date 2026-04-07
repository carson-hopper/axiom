#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Loads extractor JSON data and sends registry data + tags
	 * during the configuration phase. Caches everything on first load.
	 */
	class RegistryDataService {
	public:
		void LoadAll(const std::string& dataDirectory);

		void SendRegistries(Ref<Connection> connection);
		void SendTags(Ref<Connection> connection);

	private:
		using IdMap = std::unordered_map<std::string, int32_t>;

		void LoadSyncedRegistries(const std::string& dataDirectory);
		void LoadTags(const std::string& dataDirectory);
		void BuildIdMaps(const std::string& dataDirectory);

		IdMap BuildBlockIdMap(const nlohmann::json& blocksJson);
		IdMap BuildItemIdMap(const nlohmann::json& itemsJson);
		IdMap BuildEntityIdMap(const nlohmann::json& entitiesJson);
		IdMap BuildFluidIdMap(const nlohmann::json& fluidsJson);
		IdMap BuildGameEventIdMap(const nlohmann::json& eventsJson);
		IdMap BuildPotionIdMap(const nlohmann::json& potionsJson);
		IdMap BuildPointOfInterestIdMap();

		nlohmann::json LoadJson(const std::string& path);

		nlohmann::json m_SyncedRegistries;
		nlohmann::json m_Tags;
		std::unordered_map<std::string, IdMap> m_IdMaps;
		bool m_Loaded = false;
	};

}
