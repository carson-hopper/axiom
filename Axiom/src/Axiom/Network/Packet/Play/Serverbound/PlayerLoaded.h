#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class PlayerLoadedPacket : public Packet<PlayerLoadedPacket,
    PID_PLAY_SB_PLAYERLOADED> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>& connection, PacketContext& /*context*/, NetworkBuffer&) {
        AX_CORE_INFO("Player fully loaded from {}",
            connection->RemoteAddress());

        return std::nullopt;
    }

	AX_START_FIELDS()

	AX_END_FIELDS()
};

} // namespace Axiom::Play::Serverbound
