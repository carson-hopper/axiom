#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Nbt/JsonToNbt.h"
#include "Axiom/Config/ServerConfig.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace Axiom::Configuration::Serverbound {

	inline void SendRegistryData(const Ref<Connection>& connection, const std::string& registryId, const nlohmann::json& entries) {
		NetworkBuffer payload;
		payload.WriteString(registryId);

		const int32_t entryCount =
			static_cast<int32_t>(entries.size());
		payload.WriteVarInt(entryCount);

		for (auto& [entryName, entryData] : entries.items()) {
			std::string fullName = "minecraft:" + entryName;
			payload.WriteString(fullName);
			payload.WriteBoolean(true);

			auto nbtData =
				JsonToNbt::ObjectToRootCompound(entryData);
			payload.WriteBytes(nbtData);
		}

		connection->SendRawPacket(
			Clientbound::Config::RegistryData, payload);
	}

	nlohmann::json LoadExtractorJson(
		const std::string& fileName) {
		std::string path = "data/" + fileName;
		std::ifstream file(path);
		if (!file.good()) {
			AX_CORE_WARN("Missing extractor data: {}", path);
			return nlohmann::json::object();
		}
		return nlohmann::json::parse(file);
	}

class SelectKnownPacksPacket : public Packet<SelectKnownPacksPacket,
    PID_CONFIGURATION_SB_SELECTKNOWNPACKS> {
public:
    /**
     * Manual parse because the known
     * packs list uses variable-length
     * struct reads.
     */
    std::optional<std::vector<Ref<IChainablePacket>>>
    HandleImpl(const Ref<Connection> connection,
               PacketContext& /*context*/,
               NetworkBuffer& buffer) override {
        const int32_t count = buffer.ReadVarInt();

        struct KnownPack {
            std::string namespaceName;
            std::string identifier;
            std::string version;
        };
        std::vector<KnownPack> knownPacks(count);
        for (auto& [namespaceName, identifier, version] : knownPacks) {
            namespaceName = buffer.ReadString(256);
            identifier = buffer.ReadString(256);
            version = buffer.ReadString(256);
        }

        AX_CORE_TRACE("Client selected {} known packs",
            knownPacks.size());

        auto synced =
            LoadExtractorJson("synced_registries.json");
        for (auto& [registryName, registryEntries] : synced.items()) {
            std::string fullRegistryName =
                "minecraft:" + registryName;
            SendRegistryData(connection, fullRegistryName,
                registryEntries);
        }

        {
            NetworkBuffer tagsPayload;
            tagsPayload.WriteVarInt(0);
            connection->SendRawPacket(
                Clientbound::Config::UpdateTags, tagsPayload);
        }

        {
            const NetworkBuffer finishPayload;
            connection->SendRawPacket(
                Clientbound::Config::FinishConfiguration,
                finishPayload);
        }

        return std::nullopt;
    }

    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(Ref<Connection>, PacketContext&) { return std::nullopt; }

	AX_START_FIELDS()

	AX_END_FIELDS()
};

} // namespace Axiom::Configuration::Serverbound
