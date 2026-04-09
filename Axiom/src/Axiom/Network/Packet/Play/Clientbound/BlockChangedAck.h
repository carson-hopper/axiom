#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Play::Clientbound {

class BlockChangedAckPacket : public Packet<BlockChangedAckPacket,
	PID_PLAY_CB_BLOCKCHANGEDACK> {
public:
	BlockChangedAckPacket() = default;
	BlockChangedAckPacket(int32_t sequence) { m_Sequence.Value = sequence; }

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		buffer.WriteVarInt(m_Sequence.Value);
	}

	AX_START_FIELDS()
		AX_DECLARE(Sequence)
	AX_END_FIELDS()

	AX_FIELD(Sequence, int32_t)
};

} // namespace Axiom::Play::Clientbound
