#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Play::Serverbound {

/**
 * Plugin channel message from the client.
 * Common channels: minecraft:brand, minecraft:register, etc.
 */
class CustomPayloadPacket : public Packet<CustomPayloadPacket, PID_PLAY_SB_CUSTOMPAYLOAD> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) {
    	AX_CORE_INFO("Channel {}", m_Channel.GetValue());
        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(Channel)
    AX_END_FIELDS()

    AX_FIELD(Channel, Net::String)
};

} // namespace Axiom::Play::Serverbound
