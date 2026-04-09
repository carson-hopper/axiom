#pragma once

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class MovePlayerPositionPacket : public Packet<MovePlayerPositionPacket,
    PID_PLAY_SB_MOVEPLAYERPOSITION> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& context, NetworkBuffer&) {
        context.ChunkManagement().OnPlayerMove(connection, m_Position.Value.x, m_Position.Value.z);

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(Position),
        AX_DECLARE(OnGround)
    AX_END_FIELDS()

    AX_FIELD(Position, Vector3);
    AX_FIELD(OnGround, uint8_t);
};

} // namespace Axiom::Play::Serverbound
