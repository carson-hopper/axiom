#include "RegistryDataService.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Nbt/JsonToNbt.h"

#include <fstream>

namespace Axiom {

	void RegistryDataService::LoadAll(const std::string& dataDirectory) {
		if (m_Loaded) return;

		LoadSyncedRegistries(dataDirectory);
		LoadTags(dataDirectory);
		BuildIdMaps(dataDirectory);

		m_Loaded = true;
		AX_CORE_INFO("Loaded registry data from {}", dataDirectory);
	}

	void RegistryDataService::SendRegistries(Ref<Connection> connection) {
		for (auto& [registryName, registryEntries] : m_SyncedRegistries.items()) {
			NetworkBuffer payload;
			std::string fullName = "minecraft:" + registryName;
			payload.WriteString(fullName);

			int32_t entryCount = static_cast<int32_t>(registryEntries.size());
			payload.WriteVarInt(entryCount);

			for (auto& [entryName, entryData] : registryEntries.items()) {
				std::string fullEntryName = "minecraft:" + entryName;
				payload.WriteString(fullEntryName);
				payload.WriteBoolean(true);
				auto nbtData = JsonToNbt::ObjectToRootCompound(entryData);
				payload.WriteBytes(nbtData);
			}

			connection->SendRawPacket(Clientbound::Config::RegistryData, payload);
		}
	}

	void RegistryDataService::SendTags(Ref<Connection> connection) {
		NetworkBuffer payload;

		int32_t groupCount = static_cast<int32_t>(m_Tags.size());
		payload.WriteVarInt(groupCount);

		for (auto& [registryName, tagMap] : m_Tags.items()) {
			std::string fullRegistryName = "minecraft:" + registryName;
			payload.WriteString(fullRegistryName);

			int32_t tagCount = static_cast<int32_t>(tagMap.size());
			payload.WriteVarInt(tagCount);

			// Find the ID map for this registry
			IdMap* idMap = nullptr;
			auto mapIterator = m_IdMaps.find(registryName);
			if (mapIterator != m_IdMaps.end()) {
				idMap = &mapIterator->second;
			}

			for (auto& [tagName, tagEntries] : tagMap.items()) {
				payload.WriteString(tagName);

				if (!tagEntries.is_array()) {
					payload.WriteVarInt(0);
					continue;
				}

				int32_t entryCount = static_cast<int32_t>(tagEntries.size());
				payload.WriteVarInt(entryCount);

				for (const auto& entry : tagEntries) {
					std::string entryName = entry.get<std::string>();
					int32_t entryId = 0;

					if (idMap) {
						auto idIterator = idMap->find(entryName);
						if (idIterator != idMap->end()) {
							entryId = idIterator->second;
						} else {
							AX_CORE_WARN("Unknown tag entry: {}/{}", registryName, entryName);
						}
					}

					payload.WriteVarInt(entryId);
				}
			}
		}

		connection->SendRawPacket(Clientbound::Config::UpdateTags, payload);
	}

	void RegistryDataService::LoadSyncedRegistries(const std::string& dataDirectory) {
		m_SyncedRegistries = LoadJson(dataDirectory + "/synced_registries.json");
	}

	void RegistryDataService::LoadTags(const std::string& dataDirectory) {
		m_Tags = LoadJson(dataDirectory + "/tags.json");
	}

	void RegistryDataService::BuildIdMaps(const std::string& dataDirectory) {
		// Synced registries — entries are ordered by their JSON position
		for (auto& [registryName, entries] : m_SyncedRegistries.items()) {
			IdMap idMap;
			int32_t index = 0;
			for (auto& [entryName, entryData] : entries.items()) {
				idMap[entryName] = index++;
			}
			m_IdMaps[registryName] = std::move(idMap);
		}

		// Non-synced registries from extractor data
		auto blocksJson = LoadJson(dataDirectory + "/blocks.json");
		if (!blocksJson.is_null() && blocksJson.contains("blocks")) {
			m_IdMaps["block"] = BuildBlockIdMap(blocksJson);
		}

		auto itemsJson = LoadJson(dataDirectory + "/items.json");
		if (!itemsJson.is_null()) {
			m_IdMaps["item"] = BuildItemIdMap(itemsJson);
		}

		auto entitiesJson = LoadJson(dataDirectory + "/entities.json");
		if (!entitiesJson.is_null()) {
			m_IdMaps["entity_type"] = BuildEntityIdMap(entitiesJson);
		}

		auto fluidsJson = LoadJson(dataDirectory + "/fluids.json");
		if (!fluidsJson.is_null()) {
			m_IdMaps["fluid"] = BuildFluidIdMap(fluidsJson);
		}

		auto eventsJson = LoadJson(dataDirectory + "/game_event.json");
		if (!eventsJson.is_null()) {
			m_IdMaps["game_event"] = BuildGameEventIdMap(eventsJson);
		}

		auto potionsJson = LoadJson(dataDirectory + "/potion.json");
		if (!potionsJson.is_null()) {
			m_IdMaps["potion"] = BuildPotionIdMap(potionsJson);
		}

		m_IdMaps["point_of_interest_type"] = BuildPointOfInterestIdMap();
	}

	RegistryDataService::IdMap RegistryDataService::BuildBlockIdMap(const nlohmann::json& blocksJson) {
		IdMap map;
		for (const auto& block : blocksJson["blocks"]) {
			std::string name = block["name"].get<std::string>();
			int32_t identifier = block["id"].get<int32_t>();
			map[name] = identifier;
		}
		return map;
	}

	RegistryDataService::IdMap RegistryDataService::BuildItemIdMap(const nlohmann::json& itemsJson) {
		IdMap map;
		if (itemsJson.is_array()) {
			for (const auto& item : itemsJson) {
				std::string name = item["name"].get<std::string>();
				int32_t identifier = item["id"].get<int32_t>();
				map[name] = identifier;
			}
		} else if (itemsJson.is_object()) {
			int32_t index = 0;
			for (auto& [name, data] : itemsJson.items()) {
				if (data.is_object() && data.contains("id")) {
					map[name] = data["id"].get<int32_t>();
				} else {
					map[name] = index;
				}
				index++;
			}
		}
		return map;
	}

	RegistryDataService::IdMap RegistryDataService::BuildEntityIdMap(const nlohmann::json& entitiesJson) {
		IdMap map;
		if (entitiesJson.is_array()) {
			for (const auto& entity : entitiesJson) {
				std::string name = entity["name"].get<std::string>();
				int32_t identifier = entity["id"].get<int32_t>();
				map[name] = identifier;
			}
		} else if (entitiesJson.is_object()) {
			for (auto& [name, data] : entitiesJson.items()) {
				if (data.is_object() && data.contains("id")) {
					map[name] = data["id"].get<int32_t>();
				}
			}
		}
		return map;
	}

	RegistryDataService::IdMap RegistryDataService::BuildFluidIdMap(const nlohmann::json& fluidsJson) {
		IdMap map;
		if (fluidsJson.is_array()) {
			for (const auto& fluid : fluidsJson) {
				std::string name = fluid["name"].get<std::string>();
				int32_t identifier = fluid["id"].get<int32_t>();
				map[name] = identifier;
			}
		}
		return map;
	}

	RegistryDataService::IdMap RegistryDataService::BuildGameEventIdMap(const nlohmann::json& eventsJson) {
		IdMap map;
		if (eventsJson.is_array()) {
			int32_t index = 0;
			for (const auto& event : eventsJson) {
				map[event.get<std::string>()] = index++;
			}
		}
		return map;
	}

	RegistryDataService::IdMap RegistryDataService::BuildPotionIdMap(const nlohmann::json& potionsJson) {
		IdMap map;
		if (potionsJson.is_array()) {
			for (const auto& potion : potionsJson) {
				std::string name = potion["name"].get<std::string>();
				int32_t identifier = potion["id"].get<int32_t>();
				map[name] = identifier;
			}
		}
		return map;
	}

	RegistryDataService::IdMap RegistryDataService::BuildPointOfInterestIdMap() {
		// Hardcoded — small fixed set
		IdMap map;
		map["armorer"] = 0; map["butcher"] = 1; map["cartographer"] = 2;
		map["cleric"] = 3; map["farmer"] = 4; map["fisherman"] = 5;
		map["fletcher"] = 6; map["leatherworker"] = 7; map["librarian"] = 8;
		map["mason"] = 9; map["nitwit"] = 10; map["shepherd"] = 11;
		map["toolsmith"] = 12; map["weaponsmith"] = 13; map["home"] = 14;
		map["meeting"] = 15; map["beehive"] = 16; map["bee_nest"] = 17;
		map["nether_portal"] = 18; map["lodestone"] = 19; map["lightning_rod"] = 20;
		return map;
	}

	nlohmann::json RegistryDataService::LoadJson(const std::string& path) {
		std::ifstream file(path);
		if (!file.good()) {
			AX_CORE_WARN("Missing data file: {}", path);
			return nlohmann::json();
		}
		try {
			return nlohmann::json::parse(file);
		} catch (const nlohmann::json::exception& exception) {
			AX_CORE_ERROR("Failed to parse {}: {}", path, exception.what());
			return nlohmann::json();
		}
	}

}
