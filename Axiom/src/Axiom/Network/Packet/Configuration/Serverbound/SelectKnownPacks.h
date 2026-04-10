#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/CustomPayload.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/RegistryData.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/UpdateTags.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/FinishConfiguration.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace Axiom::Configuration::Serverbound {

struct KnownPack {
	std::string Namespace;
	std::string Identifier;
	std::string Version;
};

/** Auto-parseable network type for the known packs list. */
class KnownPackList : public Net::NetworkType<std::vector<KnownPack>> {
public:
	using Net::NetworkType<std::vector<KnownPack>>::NetworkType;

protected:
	std::vector<KnownPack> ReadImpl(NetworkBuffer& buffer) override {
		const int32_t count = buffer.ReadVarInt();
		std::vector<KnownPack> packs(count);
		for (auto& pack : packs) {
			pack.Namespace = buffer.ReadString(256);
			pack.Identifier = buffer.ReadString(256);
			pack.Version = buffer.ReadString(256);
		}
		return packs;
	}

	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteVarInt(static_cast<int32_t>(m_Value.size()));
		for (const auto& pack : m_Value) {
			buffer.WriteString(pack.Namespace);
			buffer.WriteString(pack.Identifier);
			buffer.WriteString(pack.Version);
		}
	}
};

class SelectKnownPacksPacket : public Packet<SelectKnownPacksPacket,
	PID_CONFIGURATION_SB_SELECTKNOWNPACKS> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) {
		AX_CORE_TRACE("Client selected {} known packs", m_KnownPacks.Value.GetValue().size());

		std::vector<Ref<IChainablePacket>> chain;

		auto synced = LoadExtractorJson("synced_registries.json");
		for (auto& [registryName, registryEntries] : synced.items()) {
			chain.push_back(CreateRef<Clientbound::RegistryDataPacket>(
				"minecraft:" + registryName, registryEntries));
		}

		chain.push_back(CreateRef<Clientbound::UpdateTagsPacket>());

		// Probe for Meteor client: ping the meteor-client:play channel
		// and log whatever the client echoes back.
		chain.push_back(CreateRef<Clientbound::CustomPayloadPacket>(
			"meteor-client:play"));

		chain.push_back(CreateRef<Clientbound::FinishConfigurationPacket>());
		return chain;
	}

	AX_START_FIELDS()
		AX_DECLARE(KnownPacks)
	AX_END_FIELDS()

	AX_FIELD(KnownPacks, KnownPackList)

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
