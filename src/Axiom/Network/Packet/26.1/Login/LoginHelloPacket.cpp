#include "LoginHelloPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Config/ServerConfig.h"

namespace Axiom {

	// ----- Packet Decoding ------------------------------------------

	PACKET_DECODE_BEGIN(LoginHelloPacket)
		READ_STRING_MAX(m_PlayerName, 16);
	PACKET_DECODE_END()

	// ----- Packet Handling ------------------------------------------

	PACKET_HANDLE_BEGIN(LoginHelloPacket)
		AX_CORE_INFO("Login request from {} ({})", m_PlayerName, connection->RemoteAddress());

		if (context.Config().OnlineMode()) {
			const auto verifyToken = context.GenerateVerifyToken();
			context.StorePendingLogin(connection->Id(), PendingLogin{m_PlayerName, verifyToken});

			NetworkBuffer payload;
			payload.WriteString("");  // Server ID (empty)
			payload.WriteVarInt(static_cast<int32_t>(context.KeyPair().PublicKeyDer().size()));
			payload.WriteBytes(context.KeyPair().PublicKeyDer());
			payload.WriteVarInt(static_cast<int32_t>(verifyToken.size()));
			payload.WriteBytes(verifyToken.data(), verifyToken.size());
			payload.WriteBoolean(true);  // Should authenticate

			connection->SendRawPacket(Clientbound::Login::Hello, payload);
		} else {
			std::string offlineUuid = "00000000-0000-3000-8000-" + m_PlayerName.substr(0, 12);
			while (offlineUuid.size() < 36) {
				offlineUuid += "0";
			}
			context.CompleteLogin(connection, offlineUuid, m_PlayerName);
		}
	PACKET_HANDLE_END()

	// ----- Template Instantiation -----------------------------------

	PACKET_INSTANTIATE(LoginHelloPacket, 775)

}