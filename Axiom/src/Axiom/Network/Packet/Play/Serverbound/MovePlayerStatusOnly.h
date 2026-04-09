#pragma once

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class MovePlayerStatusOnlyPacket : public Packet<MovePlayerStatusOnlyPacket,
    PID_PLAY_SB_MOVEPLAYERSTATUSONLY> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>& /*connection*/, PacketContext& /*context*/, NetworkBuffer&) {
        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(OnGround)
    AX_END_FIELDS()

    AX_FIELD(OnGround, uint8_t);
};

} // namespace Axiom::Play::Serverbound
