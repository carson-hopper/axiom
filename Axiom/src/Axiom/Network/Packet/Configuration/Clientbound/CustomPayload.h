#pragma once

#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

#include <string>
#include <utility>
#include <vector>

namespace Axiom::Configuration::Clientbound {

/**
 * Plugin channel message sent to the client during the Configuration
 * state. Typical channels include minecraft:brand and any mod-specific
 * identifiers (e.g. meteor-client:play, forge:handshake).
 *
 * Format: String channel name + remaining bytes of payload data.
 */
class CustomPayloadPacket : public Packet<CustomPayloadPacket,
	PID_CONFIGURATION_CB_CUSTOMPAYLOAD> {
public:
	CustomPayloadPacket() = default;

	CustomPayloadPacket(std::string channel, std::vector<uint8_t> payload = {})
		: m_Channel(std::move(channel)), m_Payload(std::move(payload)) {}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		buffer.WriteString(m_Channel);
		buffer.WriteBytes(m_Payload);
	}

	auto Fields() { return std::tuple<>(); }

private:
	std::string m_Channel;
	std::vector<uint8_t> m_Payload;
};

} // namespace Axiom::Configuration::Clientbound
