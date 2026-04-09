#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Core/Error.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Play/Clientbound/BlockChangedAck.h"
#include "Axiom/Environment/Level/Generator/BlockStates.h"

namespace Axiom::Play::Serverbound {

class UseItemOnPacket : public Packet<UseItemOnPacket, PID_PLAY_SB_USEITEMON> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer& buffer) {
		m_Hand.Value = buffer.ReadVarInt();
		buffer.ReadBlockPosition(m_BlockX.Value, m_BlockY.Value, m_BlockZ.Value);
		m_Face.Value = buffer.ReadVarInt();
		m_CursorX.Value = buffer.ReadFloat();
		m_CursorY.Value = buffer.ReadFloat();
		m_CursorZ.Value = buffer.ReadFloat();
		m_InsideBlock.Value = buffer.ReadBoolean();
		m_WorldBorderHit.Value = buffer.ReadBoolean();
		m_Sequence.Value = buffer.ReadVarInt();

		int32_t placeX = m_BlockX.Value;
		int32_t placeY = m_BlockY.Value;
		int32_t placeZ = m_BlockZ.Value;

		switch (m_Face.Value) {
			case 0: placeY--; break;
			case 1: placeY++; break;
			case 2: placeZ--; break;
			case 3: placeZ++; break;
			case 4: placeX--; break;
			case 5: placeX++; break;
			default:
				AX_CORE_WARN("Invalid block face {} from {}", m_Face.Value, connection->RemoteAddress());
				break;
		}

		const auto player = context.Players().GetPlayer(connection->Id());
		int32_t blockState = 0;

		if (player) {
			blockState = context.ItemBlocks().GetBlockState(player->GetHeldItemId());
		}

		if (blockState == 0 && player) {
			constexpr int32_t WaterBucketItem = 1014;
			constexpr int32_t LavaBucketItem = 1015;
			const int32_t heldItemId = player->GetHeldItemId();

			if (heldItemId == WaterBucketItem) blockState = BlockState::Water;
			else if (heldItemId == LavaBucketItem) blockState = BlockState::Lava;
		}

		if (blockState == 0) {
			return CreateChainPacketsWithArgs<Play::Clientbound::BlockChangedAckPacket>(
				std::make_tuple(m_Sequence.Value));
		}

		AX_CORE_TRACE("Block place at ({}, {}, {}) state={} from {}",
			placeX, placeY, placeZ, blockState, connection->RemoteAddress());

		if (auto result = context.Ticker().SetBlock(placeX, placeY, placeZ, blockState); !result) {
			AX_CORE_WARN("Failed to place block at ({}, {}, {}): {}",
				placeX, placeY, placeZ, result.error().message());
		}

		return CreateChainPacketsWithArgs<Play::Clientbound::BlockChangedAckPacket>(
			std::make_tuple(m_Sequence.Value));
	}


	auto Fields() { return std::tuple<>(); }

	AX_FIELD(Hand, int32_t)
	AX_FIELD(BlockX, int32_t)
	AX_FIELD(BlockY, int32_t)
	AX_FIELD(BlockZ, int32_t)
	AX_FIELD(Face, int32_t)
	AX_FIELD(CursorX, float)
	AX_FIELD(CursorY, float)
	AX_FIELD(CursorZ, float)
	AX_FIELD(InsideBlock, bool)
	AX_FIELD(WorldBorderHit, bool)
	AX_FIELD(Sequence, int32_t)
};

} // namespace Axiom::Play::Serverbound
