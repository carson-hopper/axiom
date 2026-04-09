#pragma once

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class SetCarriedItemPacket : public Packet<SetCarriedItemPacket,
    PID_PLAY_SB_SETCARRIEDITEM> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& context) {
        if (const auto player =
                context.Players().GetPlayer(connection->Id())) {
            player->SetSelectedSlot(m_Slot.Value);
        }

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(Slot)
    AX_END_FIELDS()

    AX_FIELD(Slot, int16_t);
};

} // namespace Axiom::Play::Serverbound
