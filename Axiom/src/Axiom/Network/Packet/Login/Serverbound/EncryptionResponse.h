#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Core/Time.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Crypto/MinecraftHash.h"
#include "Axiom/Network/Crypto/MojangAuth.h"
#include "Axiom/Network/Packet/Login/Clientbound/LoginCompression.h"
#include "Axiom/Network/Packet/Login/Clientbound/LoginFinished.h"

#include <thread>

namespace Axiom::Login::Serverbound {

class EncryptionResponsePacket : public Packet<EncryptionResponsePacket,
	PID_LOGIN_SB_ENCRYPTIONRESPONSE> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer&) {
		std::vector<uint8_t> sharedSecret;
		std::vector<uint8_t> verifyToken;
		try {
			sharedSecret = context.KeyPair().Decrypt(m_EncryptedSharedSecret.Value.GetValue());
			verifyToken = context.KeyPair().Decrypt(m_EncryptedVerifyToken.Value.GetValue());
		} catch (const std::exception& exception) {
			AX_CORE_ERROR("Decryption failed for {}: {}", connection->RemoteAddress(), exception.what());
			connection->Disconnect("Encryption error");
			return std::nullopt;
		}

		auto pendingLogin = context.TakePendingLogin(connection->Id());
		if (!pendingLogin) {
			connection->Disconnect("No pending login");
			return std::nullopt;
		}

		if (verifyToken.size() != pendingLogin->verifyToken.size()
			|| !std::equal(verifyToken.begin(), verifyToken.end(), pendingLogin->verifyToken.begin())) {
			connection->Disconnect("Verify token mismatch");
			return std::nullopt;
		}

		connection->EnableEncryption(sharedSecret);

		const auto& connectionRef = connection;
		std::string playerName = pendingLogin->playerName;
		auto publicKey = context.KeyPair().PublicKeyDer();

		std::thread([&context, connectionRef, playerName, sharedSecret, publicKey]() {
			const std::string serverHash = MinecraftServerHash("", sharedSecret, publicKey);

			auto profile = MojangAuth::HasJoined(playerName, serverHash);
			if (!profile) {
				connectionRef->Disconnect("Failed to verify username");
				return;
			}

			AX_CORE_INFO("Authenticated {} (UUID: {})", profile->name, profile->uuid);

			std::string formattedUuid = context.FormatUuid(profile->uuid);

			// Send compression (OnSent enables it)
			Clientbound::LoginCompressionPacket compressionPacket(256);
			NetworkBuffer compressionPayload;
			compressionPacket.Write(compressionPayload);
			connectionRef->SendRawPacket(compressionPacket.GetPacketId(), compressionPayload);
			compressionPacket.OnSent(connectionRef);

			// Send login finished
			Clientbound::LoginFinishedPacket finishedPacket(formattedUuid, profile->name);
			NetworkBuffer finishedPayload;
			finishedPacket.Write(finishedPayload);
			connectionRef->SendRawPacket(finishedPacket.GetPacketId(), finishedPayload);

			// Register player with skin properties
			UUID playerUuid = UUID::FromString(formattedUuid);
			auto player = context.Server().AddPlayer(
				connectionRef, profile->name, playerUuid);
			player->SetPosition({0.5, context.ChunkManagement().Generator().SpawnY(), 0.5});
			player->SetOpLevel(static_cast<OpLevel>(
				context.AdminFiles().OpLevel(playerUuid.ToString())));

			// Refresh the usercache so offline-player
			// lookups (ban/op/whitelist by name) work.
			// Entries live for 7 days before expiring.
			UserCacheEntry cacheEntry;
			cacheEntry.Uuid = playerUuid.ToString();
			cacheEntry.Name = profile->name;
			cacheEntry.ExpiresOn = Time::Now().AddDays(7);
			context.AdminFiles().UpdateCache(cacheEntry);

			// Store Mojang properties (textures/skin)
			std::vector<PlayerProperty> properties;
			for (const auto& prop : profile->properties) {
				properties.push_back({
					prop.value("name", ""),
					prop.value("value", ""),
					prop.value("signature", "")
				});
			}
			player->SetProperties(std::move(properties));
		}).detach();

		return std::nullopt;
	}

	AX_START_FIELDS()
		AX_DECLARE(EncryptedSharedSecret),
		AX_DECLARE(EncryptedVerifyToken)
	AX_END_FIELDS()

	AX_FIELD(EncryptedSharedSecret, Net::ByteArray)
	AX_FIELD(EncryptedVerifyToken, Net::ByteArray)
};

} // namespace Axiom::Login::Serverbound
