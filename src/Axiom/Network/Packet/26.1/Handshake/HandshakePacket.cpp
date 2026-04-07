#include "HandshakePacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	template<int32_t Version>
	void HandshakePacket<Version>::Handle(const Ref<Connection> connection, PacketContext& /*context*/) {
		AX_CORE_TRACE("Handshake from {}: protocol={}, address={}:{}, nextState={}",
			connection->RemoteAddress(), protocolVersion, serverAddress, serverPort, nextState);

		connection->ProtocolVersion(protocolVersion);

		if (nextState == 1) {
			connection->State(ConnectionState::Status);
		} else if (nextState == 2) {
			connection->State(ConnectionState::Login);
		} else {
			connection->Disconnect("Invalid next state");
		}
	}

	template class HandshakePacket<775>;

}
