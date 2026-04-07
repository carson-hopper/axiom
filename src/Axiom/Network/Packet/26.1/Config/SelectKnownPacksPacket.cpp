#include "SelectKnownPacksPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Nbt/JsonToNbt.h"
#include "Axiom/Config/ServerConfig.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace Axiom {

	namespace {

		void SendRegistryData(Ref<Connection>& connection, const std::string& registryId,
			const nlohmann::json& entries) {

			NetworkBuffer payload;
			payload.WriteString(registryId);

			int32_t entryCount = static_cast<int32_t>(entries.size());
			payload.WriteVarInt(entryCount);

			for (auto& [entryName, entryData] : entries.items()) {
				std::string fullName = "minecraft:" + entryName;
				payload.WriteString(fullName);

				// Has data = true
				payload.WriteBoolean(true);

				// Encode entry as NBT
				auto nbtData = JsonToNbt::ObjectToRootCompound(entryData);
				payload.WriteBytes(nbtData);
			}

			connection->SendRawPacket(Clientbound::Config::RegistryData, payload);
		}

		nlohmann::json LoadExtractorJson(const std::string& fileName) {
			std::string path = "data/" + fileName;
			std::ifstream file(path);
			if (!file.good()) {
				AX_CORE_WARN("Missing extractor data: {}", path);
				return nlohmann::json::object();
			}
			return nlohmann::json::parse(file);
		}

	} // anonymous namespace

	template<int32_t Version>
	void SelectKnownPacksPacket<Version>::Handle(Ref<Connection> connection, PacketContext& context) {
		AX_CORE_TRACE("Client selected {} known packs", knownPacks.size());

		// Send registry data from extractor files
		// The client needs these registries to proceed through configuration
		auto synced = LoadExtractorJson("synced_registries.json");

		for (auto& [registryName, registryEntries] : synced.items()) {
			std::string fullRegistryName = "minecraft:" + registryName;
			SendRegistryData(connection, fullRegistryName, registryEntries);
		}

		// Send UpdateTags (empty for now)
		{
			NetworkBuffer payload;
			payload.WriteVarInt(0);  // No tag groups
			connection->SendRawPacket(Clientbound::Config::UpdateTags, payload);
		}

		// Send FinishConfiguration
		{
			NetworkBuffer payload;
			connection->SendRawPacket(Clientbound::Config::FinishConfiguration, payload);
		}
	}

	template class SelectKnownPacksPacket<775>;

}
