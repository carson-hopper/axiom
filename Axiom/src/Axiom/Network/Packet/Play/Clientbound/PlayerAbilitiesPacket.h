#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Play::Clientbound {

/**
 * Tells the client about its current
 * ability flags, fly speed, and the
 * field-of-view walk speed modifier.
 */
class PlayerAbilitiesPacket : public Packet<PlayerAbilitiesPacket,
    PID_PLAY_CB_PLAYERABILITIES> {
public:
    PlayerAbilitiesPacket() = default;

    PlayerAbilitiesPacket(int8_t flags, float flySpeed, float walkSpeed) {
        m_Flags.Value = flags;
        m_FlySpeed.Value = flySpeed;
        m_WalkSpeed.Value = walkSpeed;
    }

    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

    AX_START_FIELDS()
        AX_DECLARE(Flags),
        AX_DECLARE(FlySpeed),
        AX_DECLARE(WalkSpeed)
    AX_END_FIELDS()

    AX_FIELD(Flags, Net::Byte)
    AX_FIELD(FlySpeed, Net::Float)
    AX_FIELD(WalkSpeed, Net::Float)
};

} // namespace Axiom::Play::Clientbound
