#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/RegistryData.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/UpdateTags.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/FinishConfiguration.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace Axiom::Configuration::Serverbound {

class SelectKnownPacksPacket : public Packet<SelectKnownPacksPacket,
	PID_CONFIGURATION_SB_SELECTKNOWNPACKS> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext&,
	           NetworkBuffer& buffer) {

		const int32_t count = buffer.ReadVarInt();
		for (int32_t i = 0; i < count; i++) {
			buffer.ReadString(256); // namespace
			buffer.ReadString(256); // identifier
			buffer.ReadString(256); // version
		}

		AX_CORE_TRACE("Client selected {} known packs", count);

		// Build chain: RegistryData × N + UpdateTags + FinishConfiguration
		std::vector<Ref<IChainablePacket>> chain;

		auto synced = LoadExtractorJson("synced_registries.json");
		for (auto& [registryName, registryEntries] : synced.items()) {
			chain.push_back(CreateRef<Clientbound::RegistryDataPacket>(
				"minecraft:" + registryName, registryEntries));
		}

		chain.push_back(CreateRef<Clientbound::UpdateTagsPacket>());
		chain.push_back(CreateRef<Clientbound::FinishConfigurationPacket>());

		return chain;
	}


	auto Fields() { return std::tuple<>(); }

private:
	static nlohmann::json LoadExtractorJson(const std::string& fileName) {
		std::string path = "data/" + fileName;
		std::ifstream file(path);
		if (!file.good()) {
			AX_CORE_WARN("Missing extractor data: {}", path);
			return nlohmann::json::object();
		}
		return nlohmann::json::parse(file);
	}
};

} // namespace Axiom::Configuration::Serverbound
