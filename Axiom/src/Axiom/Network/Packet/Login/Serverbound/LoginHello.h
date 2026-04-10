#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Core/Time.h"
#include "Axiom/Core/UUID.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/Login/Clientbound/EncryptionRequest.h"
#include "Axiom/Network/Packet/Login/Clientbound/LoginCompression.h"
#include "Axiom/Network/Packet/Login/Clientbound/LoginFinished.h"

namespace Axiom::Login::Serverbound {

class LoginHelloPacket : public Packet<LoginHelloPacket,
	PID_LOGIN_SB_LOGINHELLO> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer&) {
		AX_CORE_INFO("Login request from {} ({})",
			m_PlayerName.GetValue(), connection->RemoteAddress());

		if (context.Config().OnlineMode()) {
			const auto verifyToken = context.GenerateVerifyToken();
			context.StorePendingLogin(connection->Id(),
				PendingLogin{m_PlayerName.GetValue(), verifyToken});

			return CreateChainPacketsWithArgs<Clientbound::EncryptionRequestPacket>(
				std::make_tuple(
					std::string(""),
					context.KeyPair().PublicKeyDer(),
					std::vector<uint8_t>(verifyToken.begin(), verifyToken.end()),
					true));
		}

		// Offline mode — generate offline UUID
		std::string offlineUuidString =
			"00000000-0000-3000-8000-"
			+ m_PlayerName.GetValue().substr(0, 12);
		while (offlineUuidString.size() < 36) offlineUuidString += "0";
		UUID offlineUuid = UUID::FromString(offlineUuidString);

		// Register player
		auto player = context.Server().AddPlayer(
			connection, m_PlayerName.GetValue(), offlineUuid);
		player->SetPosition({0.5, context.ChunkManagement().Generator().SpawnY(), 0.5});
		player->SetOpLevel(static_cast<OpLevel>(
			context.AdminFiles().OpLevel(offlineUuid.ToString())));

		// Refresh the usercache so offline-player
		// lookups (ban/op/whitelist by name) work.
		// Entries live for 7 days before expiring.
		UserCacheEntry cacheEntry;
		cacheEntry.Uuid = offlineUuid.ToString();
		cacheEntry.Name = m_PlayerName.GetValue();
		cacheEntry.ExpiresOn = Time::Now().AddDays(7);
		context.AdminFiles().UpdateCache(cacheEntry);

		AX_CORE_INFO("{} has logged in [{}]",
			m_PlayerName.GetValue(), offlineUuid.ToString());

		// Chain: LoginCompression (OnSent enables compression) → LoginFinished
		std::vector<Ref<IChainablePacket>> chain;
		chain.push_back(CreateRef<Clientbound::LoginCompressionPacket>(256));
		chain.push_back(CreateRef<Clientbound::LoginFinishedPacket>(offlineUuid, m_PlayerName.GetValue()));
		return chain;
	}

	AX_START_FIELDS()
		AX_DECLARE(PlayerName)
	AX_END_FIELDS()

	AX_FIELD(PlayerName, Net::String)
};

} // namespace Axiom::Login::Serverbound
