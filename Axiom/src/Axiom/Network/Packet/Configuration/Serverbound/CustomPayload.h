#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Configuration::Serverbound {

/**
 * Plugin channel message from the client during the Configuration
 * state. Used for mod handshake protocols (Forge, Fabric, Meteor,
 * etc.) before entering the Play state.
 */
class CustomPayloadPacket : public Packet<CustomPayloadPacket,
	PID_CONFIGURATION_SB_CUSTOMPAYLOAD> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext&, NetworkBuffer& buffer) {
		const std::string& channel = m_Channel.Value.GetValue();
		const size_t payloadSize = buffer.ReadableBytes();
		AX_CORE_INFO("[Config] CustomPayload channel={} from {} ({} bytes)",
			channel, connection->RemoteAddress(), payloadSize);

		// minecraft:register / minecraft:unregister carry a list of
		// null-terminated channel names the client wants to (un)subscribe.
		if (channel == "minecraft:register" || channel == "minecraft:unregister") {
			auto payload = buffer.ReadBytes(payloadSize);
			std::string current;
			std::vector<std::string> channels;
			for (const uint8_t byte : payload) {
				if (byte == 0) {
					if (!current.empty()) {
						channels.push_back(std::move(current));
						current.clear();
					}
				} else {
					current.push_back(static_cast<char>(byte));
				}
			}
			if (!current.empty()) {
				channels.push_back(std::move(current));
			}

			AX_CORE_INFO("[Config] Client registered {} channel(s):", channels.size());
			for (const auto& channel : channels) {
				AX_CORE_INFO("  - {}", channel);
			}
		}

		return std::nullopt;
	}

	AX_START_FIELDS()
		AX_DECLARE(Channel)
	AX_END_FIELDS()

	AX_FIELD(Channel, Net::String)
};

} // namespace Axiom::Configuration::Serverbound
