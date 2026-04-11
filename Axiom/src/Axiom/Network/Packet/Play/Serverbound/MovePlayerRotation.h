#pragma once

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class MovePlayerRotationPacket : public Packet<MovePlayerRotationPacket,
    PID_PLAY_SB_MOVEPLAYERROTATION> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer&) {
    	const Ref<Player> player = context.Server().GetPlayer(connection);
    	if (player != nullptr) {
    		player->SetRotation(m_Rotation.Value);
    		player->SetOnGround(m_OnGround.Value);
    	}

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(Rotation),
        AX_DECLARE(OnGround)
    AX_END_FIELDS()

    AX_FIELD(Rotation, Vector2);
    AX_FIELD(OnGround, uint8_t);
};

} // namespace Axiom::Play::Serverbound
