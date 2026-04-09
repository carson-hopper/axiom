#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Core/Error.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Play/Clientbound/BlockChangedAck.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"

namespace Axiom::Play::Serverbound {

class PlayerActionPacket : public Packet<PlayerActionPacket, PID_PLAY_SB_PLAYERACTION> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer& buffer) {
		m_Action.Value = buffer.ReadVarInt();
		buffer.ReadBlockPosition(m_BlockX.Value, m_BlockY.Value, m_BlockZ.Value);
		m_Face.Value = buffer.ReadByte();
		m_Sequence.Value = buffer.ReadVarInt();

		if (m_Action.Value == 0 || m_Action.Value == 2) {
			AX_CORE_TRACE("Block break at ({}, {}, {}) from {}",
				m_BlockX.Value, m_BlockY.Value, m_BlockZ.Value, connection->RemoteAddress());

			auto result = context.Ticker().SetBlock(m_BlockX.Value, m_BlockY.Value, m_BlockZ.Value, BlockState::Air);
			if (!result) {
				AX_CORE_WARN("Failed to break block at ({}, {}, {}): {}",
					m_BlockX.Value, m_BlockY.Value, m_BlockZ.Value, result.error().message());
			}

			return CreateChainPacketsWithArgs<Play::Clientbound::BlockChangedAckPacket>(
				std::make_tuple(m_Sequence.Value));
		}

		return std::nullopt;
	}


	auto Fields() { return std::tuple<>(); }

	AX_FIELD(Action, int32_t)
	AX_FIELD(BlockX, int32_t)
	AX_FIELD(BlockY, int32_t)
	AX_FIELD(BlockZ, int32_t)
	AX_FIELD(Face, int8_t)
	AX_FIELD(Sequence, int32_t)
};

} // namespace Axiom::Play::Serverbound
