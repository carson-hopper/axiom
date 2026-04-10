#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Play::Serverbound {

/**
 * Plugin channel message from the client.
 * Common channels: minecraft:brand, minecraft:register, etc.
 */
class CustomPayloadPacket : public Packet<CustomPayloadPacket, PID_PLAY_SB_CUSTOMPAYLOAD> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>& connection, PacketContext&, NetworkBuffer& buffer) {
        const std::string& channel = m_Channel.Value.GetValue();
        const size_t payloadSize = buffer.ReadableBytes();
        AX_CORE_INFO("[Play] CustomPayload channel={} from {} ({} bytes)",
            channel, connection->RemoteAddress(), payloadSize);

        if (channel == "minecraft:register" || channel == "minecraft:unregister") {
            const auto payload = buffer.ReadBytes(payloadSize);
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

            AX_CORE_INFO("[Play] Client registered {} channel(s):", channels.size());
            for (const auto& ch : channels) {
                AX_CORE_INFO("  - {}", ch);
            }
        }
        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(Channel)
    AX_END_FIELDS()

    AX_FIELD(Channel, Net::String)
};

} // namespace Axiom::Play::Serverbound
