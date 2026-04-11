#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/CustomPayload.h"
#include "Axiom/Network/Packet/Configuration/Clientbound/FinishConfiguration.h"

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
		for (auto& [Namespace, Identifier, Version] : packs) {
			Namespace = buffer.ReadString(256);
			Identifier = buffer.ReadString(256);
			Version = buffer.ReadString(256);
		}
		return packs;
	}

	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteVarInt(static_cast<int32_t>(m_Value.size()));
		for (const auto& [Namespace, Identifier, Version] : m_Value) {
			buffer.WriteString(Namespace);
			buffer.WriteString(Identifier);
			buffer.WriteString(Version);
		}
	}
};

class SelectKnownPacksPacket : public Packet<SelectKnownPacksPacket,
	PID_CONFIGURATION_SB_SELECTKNOWNPACKS> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer&) {
		AX_CORE_TRACE("Client selected {} known packs", m_KnownPacks.Value.GetValue().size());

		// Authoritative registry + tag send through RegistryDataService.
		// Queued synchronously before the chain below so the client sees
		// them in order: registries → tags → custom payload → finish.
		context.Registries().SendRegistries(connection);
		context.Registries().SendTags(connection);

		std::vector<Ref<IChainablePacket>> chain;
		chain.push_back(Ref<Clientbound::CustomPayloadPacket>::Create(
			"meteor-client:play"));
		chain.push_back(Ref<Clientbound::FinishConfigurationPacket>::Create());
		return chain;
	}

	AX_START_FIELDS()
		AX_DECLARE(KnownPacks)
	AX_END_FIELDS()

	AX_FIELD(KnownPacks, KnownPackList)
};

} // namespace Axiom::Configuration::Serverbound
