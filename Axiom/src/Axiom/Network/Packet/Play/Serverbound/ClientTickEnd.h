#pragma once

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class ClientTickEndPacket : public Packet<ClientTickEndPacket,
    PID_PLAY_SB_CLIENTTICKEND> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>& /*connection*/, PacketContext& /*context*/, NetworkBuffer&) {
        return std::nullopt;
    }

	AX_START_FIELDS()

	AX_END_FIELDS()
};

} // namespace Axiom::Play::Serverbound
