#pragma once

#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Nbt/JsonToNbt.h"

#include <nlohmann/json.hpp>

namespace Axiom::Configuration::Clientbound {

class RegistryDataPacket : public Packet<RegistryDataPacket,
	PID_CONFIGURATION_CB_REGISTRYDATA> {
public:
	RegistryDataPacket() = default;
	RegistryDataPacket(std::string registryName, nlohmann::json entries)
		: m_RegistryName(std::move(registryName))
		, m_Entries(std::move(entries)) {}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		buffer.WriteString(m_RegistryName);
		buffer.WriteVarInt(static_cast<int32_t>(m_Entries.size()));

		for (auto& [entryName, entryData] : m_Entries.items()) {
			buffer.WriteString("minecraft:" + entryName);
			buffer.WriteBoolean(true);
			auto nbtData = JsonToNbt::ObjectToRootCompound(entryData);
			buffer.WriteBytes(nbtData);
		}
	}

	auto Fields() { return std::tuple<>(); }

private:
	std::string m_RegistryName;
	nlohmann::json m_Entries;
};

} // namespace Axiom::Configuration::Clientbound
