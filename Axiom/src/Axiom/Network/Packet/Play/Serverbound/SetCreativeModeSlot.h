#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Play::Serverbound {

class SetCreativeModeSlotPacket : public Packet<SetCreativeModeSlotPacket, PID_PLAY_SB_SETCREATIVEMODESLOT> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer& buffer) {
		m_SlotIndex.Value = buffer.ReadShort();
		m_ItemCount.Value = buffer.ReadVarInt();

		if (m_ItemCount.Value > 0) {
			m_ItemId.Value = buffer.ReadVarInt();
			const int32_t componentsToAdd = buffer.ReadVarInt();
			const int32_t componentsToRemove = buffer.ReadVarInt();
			(void)componentsToRemove;
			for (int32_t i = 0; i < componentsToAdd; i++) {
				buffer.ReadVarInt();
				break;
			}
		}

		const auto player = context.Server().GetPlayer(connection->Id());
		if (!player) return std::nullopt;

		if (m_SlotIndex.Value >= 36 && m_SlotIndex.Value <= 44) {
			const int hotbarIndex = m_SlotIndex.Value - 36;
			player->SetHotbarItem(hotbarIndex, m_ItemId.Value, m_ItemCount.Value);
			AX_CORE_TRACE("Creative slot {}: item {} x{}", hotbarIndex, m_ItemId.Value, m_ItemCount.Value);
		}

		return std::nullopt;
	}


	auto Fields() { return std::tuple<>(); }

	AX_FIELD(SlotIndex, int16_t)
	AX_FIELD(ItemCount, int32_t)
	AX_FIELD(ItemId, int32_t)
};

} // namespace Axiom::Play::Serverbound
