#pragma once

#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class SwingPacket : public Packet<SwingPacket, PID_PLAY_SB_SWING> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) {
		// Arm swing animation — no server action needed
		// TODO: broadcast Animate packet to nearby players
		return std::nullopt;
	}

	AX_START_FIELDS()
		AX_DECLARE(Hand)
	AX_END_FIELDS()

	AX_FIELD(Hand, Net::VarInt)
};

} // namespace Axiom::Play::Serverbound
