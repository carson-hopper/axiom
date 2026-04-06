#include "LoginHelloPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Config/ServerConfig.h"

namespace Axiom {

	template<int32_t Version>
	void LoginHelloPacket<Version>::Handle(Ref<Connection> connection, PacketContext& context) {
		AX_CORE_INFO("Login request from {} ({})", playerName, connection->RemoteAddress());

		if (context.Config().OnlineMode()) {
			auto verifyToken = context.GenerateVerifyToken();
			context.StorePendingLogin(connection.get(), PendingLogin{playerName, verifyToken});

			NetworkBuffer payload;
			payload.WriteString("");  // Server ID (empty)
			payload.WriteVarInt(static_cast<int32_t>(context.KeyPair().PublicKeyDer().size()));
			payload.WriteBytes(context.KeyPair().PublicKeyDer());
			payload.WriteVarInt(static_cast<int32_t>(verifyToken.size()));
			payload.WriteBytes(verifyToken.data(), verifyToken.size());
			payload.WriteBoolean(true);  // Should authenticate

			connection->SendRawPacket(Clientbound::Login::Hello, payload);
		} else {
			std::string offlineUuid = "00000000-0000-3000-8000-" + playerName.substr(0, 12);
			while (offlineUuid.size() < 36) {
				offlineUuid += "0";
			}
			context.CompleteLogin(connection, offlineUuid, playerName);
		}
	}

	template class LoginHelloPacket<775>;

}
