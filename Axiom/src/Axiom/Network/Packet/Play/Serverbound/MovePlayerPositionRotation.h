#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class MovePlayerPositionRotationPacket : public Packet<MovePlayerPositionRotationPacket,
    PID_PLAY_SB_MOVEPLAYERPOSITIONROTATION> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& context, NetworkBuffer&) {
        const Ref<Player> player = context.Server().GetPlayer(connection);
        if (player == nullptr) {
            return std::nullopt;
        }

        if (player->IsAwaitingTeleportAck()) {
            return std::nullopt;
        }

        const Vector3 newPosition = m_Position.Value;
        if (!player->IsValidMoveTarget(newPosition)) {
            AX_CORE_WARN(
                "Rejecting move+rot from {}: position ({:.2f}, {:.2f}, {:.2f}) "
                "failed validation; snapping back",
                player->Name(), newPosition.x, newPosition.y, newPosition.z);
            player->Teleport(player->GetPosition());
            return std::nullopt;
        }

        player->SetPosition(newPosition);
        player->SetRotation(m_Rotation.Value);
        player->SetOnGround(m_OnGround.Value);
        context.ChunkManagement().OnPlayerMove(connection, newPosition.x, newPosition.z);

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(Position),
        AX_DECLARE(Rotation),
        AX_DECLARE(OnGround)
    AX_END_FIELDS()

    AX_FIELD(Position, Vector3);
    AX_FIELD(Rotation, Vector2);
    AX_FIELD(OnGround, uint8_t);
};

} // namespace Axiom::Play::Serverbound
