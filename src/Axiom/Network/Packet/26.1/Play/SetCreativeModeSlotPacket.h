#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>

namespace Axiom {

	/**
	 * In creative mode, the client tells the server to put an item in a slot.
	 * We use this to track what the player is holding.
	 */
	template<int32_t Version = PROTOCOL_VERSION>
	class SetCreativeModeSlotPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::SetCreativeModeSlot;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			m_SlotIndex = buffer.ReadShort();

			// Slot data: read the item count and ID
			// In 26.1, slot format is: VarInt count, then if count > 0:
			//   VarInt itemId, VarInt componentsToAdd, VarInt componentsToRemove, ...
			m_ItemCount = buffer.ReadVarInt();
			if (m_ItemCount > 0) {
				m_ItemId = buffer.ReadVarInt();
				// Skip remaining component data
				const int32_t componentsToAdd = buffer.ReadVarInt();
				const int32_t componentsToRemove = buffer.ReadVarInt();
				// We don't need the component data for block placement
				// Just consume the rest of the buffer
				for (int32_t i = 0; i < componentsToAdd; i++) {
					buffer.ReadVarInt(); // Component type
					// Skip component data — varies by type, consume remaining
					break; // Simplified: just read the first and stop
				}
			}
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

	private:
		int16_t m_SlotIndex = 0;
		int32_t m_ItemCount = 0;
		int32_t m_ItemId = 0;
	};

}
