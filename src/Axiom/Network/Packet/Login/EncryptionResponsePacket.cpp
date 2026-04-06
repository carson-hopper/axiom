#include "EncryptionResponsePacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Crypto/MinecraftHash.h"
#include "Axiom/Network/Crypto/MojangAuth.h"

#include <thread>

namespace Axiom {

	template<int32_t Version>
	void EncryptionResponsePacket<Version>::Handle(Ref<Connection> connection, PacketContext& context) {
		std::vector<uint8_t> sharedSecret;
		std::vector<uint8_t> verifyToken;
		try {
			sharedSecret = context.KeyPair().Decrypt(encryptedSharedSecret);
			verifyToken = context.KeyPair().Decrypt(encryptedVerifyToken);
		} catch (const std::exception& exception) {
			AX_CORE_ERROR("Decryption failed for {}: {}", connection->RemoteAddress(), exception.what());
			connection->Disconnect("Encryption error");
			return;
		}

		auto pendingLogin = context.TakePendingLogin(connection.get());
		if (!pendingLogin) {
			connection->Disconnect("No pending login");
			return;
		}

		if (verifyToken.size() != pendingLogin->verifyToken.size() ||
			!std::equal(verifyToken.begin(), verifyToken.end(), pendingLogin->verifyToken.begin())) {
			connection->Disconnect("Verify token mismatch");
			return;
		}

		connection->EnableEncryption(sharedSecret);

		auto connectionRef = connection;
		std::string playerName = pendingLogin->playerName;
		auto publicKey = context.KeyPair().PublicKeyDer();

		std::thread([&context, connectionRef, playerName, sharedSecret, publicKey]() {
			std::string serverHash = MinecraftServerHash("", sharedSecret, publicKey);

			auto profile = MojangAuth::HasJoined(playerName, serverHash);
			if (!profile) {
				connectionRef->Disconnect("Failed to verify username");
				return;
			}

			AX_CORE_INFO("Authenticated {} (UUID: {})", profile->name, profile->uuid);
			context.CompleteLogin(connectionRef, context.FormatUuid(profile->uuid), profile->name);
		}).detach();
	}

	template class EncryptionResponsePacket<775>;

}
