#include "AcceptTeleportationPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void AcceptTeleportationPacket<Version>::Handle(const Ref<Connection> connection, PacketContext& /*context*/) {
		AX_CORE_TRACE("Teleport accepted from {}: id={}", connection->RemoteAddress(), teleportId);
	}

	template class AcceptTeleportationPacket<775>;

}
