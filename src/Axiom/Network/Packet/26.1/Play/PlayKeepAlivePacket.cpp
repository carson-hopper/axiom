#include "PlayKeepAlivePacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void PlayKeepAlivePacket<Version>::Handle(const Ref<Connection> connection, PacketContext& context) {
		context.KeepAlive().OnKeepAliveResponse(connection.get(), keepAliveId);
	}

	template class PlayKeepAlivePacket<775>;

}
