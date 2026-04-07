#include "SetCarriedItemPacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void SetCarriedItemPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& context) {
		auto player = context.Players().GetPlayer(connection.get());
		if (player) {
			player->SetSelectedSlot(m_Slot);
		}
	}

	template class SetCarriedItemPacket<775>;

}
