#pragma once

#include "Axiom/Data/Nbt/JsonToNbt.h"
#include "Axiom/Network/Packet/Packet.h"

#include <nlohmann/json.hpp>

namespace Axiom::Configuration::Clientbound {

/** Network type that serializes a JSON object as registry entries (VarInt count + name/NBT pairs). */
class RegistryEntries : public Net::NetworkType<nlohmann::json> {
public:
	using Net::NetworkType<nlohmann::json>::NetworkType;

protected:
	nlohmann::json ReadImpl(NetworkBuffer&) override {
		return nlohmann::json::object(); // Clientbound only — never read
	}

	void WriteImpl(NetworkBuffer& buffer) const override {
		buffer.WriteVarInt(static_cast<int32_t>(m_Value.size()));
		for (auto& [entryName, entryData] : m_Value.items()) {
			buffer.WriteString("minecraft:" + entryName);
			buffer.WriteBoolean(true);
			auto nbtData = JsonToNbt::ObjectToRootCompound(entryData);
			buffer.WriteBytes(nbtData);
		}
	}
};

class RegistryDataPacket : public Packet<RegistryDataPacket,
	PID_CONFIGURATION_CB_REGISTRYDATA> {
public:
	RegistryDataPacket() = default;
	RegistryDataPacket(std::string registryName, nlohmann::json entries) {
		m_RegistryName.Value = std::move(registryName);
		m_Entries.Value = std::move(entries);
	}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	AX_START_FIELDS()
		AX_DECLARE(RegistryName),
		AX_DECLARE(Entries)
	AX_END_FIELDS()

	AX_FIELD(RegistryName, Net::String)
	AX_FIELD(Entries, RegistryEntries)
};

} // namespace Axiom::Configuration::Clientbound
