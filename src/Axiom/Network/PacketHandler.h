#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

	class ServerConfig;

	class PacketHandler {
	public:
		explicit PacketHandler(ServerConfig& config);

		void HandlePacket(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer);

	private:
		void HandleHandshake(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer);
		void HandleStatus(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer);
		void HandleLogin(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer);

		ServerConfig& m_Config;
	};

}
