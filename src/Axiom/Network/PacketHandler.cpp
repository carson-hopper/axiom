#include "PacketHandler.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Crypto/MinecraftHash.h"
#include "Axiom/Network/Crypto/MojangAuth.h"
#include "Axiom/Config/ServerConfig.h"

#include <nlohmann/json.hpp>

#include <thread>

namespace Axiom {

	static constexpr int32_t COMPRESSION_THRESHOLD = 256;

	PacketHandler::PacketHandler(ServerConfig& config)
		: m_Config(config) {}

	void PacketHandler::HandlePacket(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer) {
		switch (connection->State()) {
			case ConnectionState::Handshake:
				HandleHandshake(connection, packetId, buffer);
				break;
			case ConnectionState::Status:
				HandleStatus(connection, packetId, buffer);
				break;
			case ConnectionState::Login:
				HandleLogin(connection, packetId, buffer);
				break;
			default:
				AX_CORE_WARN("Unhandled packet 0x{:02X} in state {}", packetId,
					static_cast<int>(connection->State()));
				break;
		}
	}

	void PacketHandler::HandleHandshake(Ref<Connection> connection, int32_t /*packetId*/, NetworkBuffer& buffer) {
		int32_t protocolVersion = buffer.ReadVarInt();
		std::string serverAddress = buffer.ReadString(255);
		uint16_t serverPort = buffer.ReadUnsignedShort();
		int32_t nextState = buffer.ReadVarInt();

		AX_CORE_TRACE("Handshake from {}: protocol={}, address={}:{}, nextState={}",
			connection->RemoteAddress(), protocolVersion, serverAddress, serverPort, nextState);

		if (nextState == 1) {
			connection->State(ConnectionState::Status);
		} else if (nextState == 2) {
			connection->State(ConnectionState::Login);
		} else {
			connection->Disconnect("Invalid next state");
		}
	}

	void PacketHandler::HandleStatus(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer) {
		if (packetId == Serverbound::Status::StatusRequest) {
			nlohmann::json response;
			response["version"]["name"] = MINECRAFT_VERSION;
			response["version"]["protocol"] = PROTOCOL_VERSION;
			response["players"]["max"] = m_Config.MaxPlayers();
			response["players"]["online"] = 0;
			response["players"]["sample"] = nlohmann::json::array();
			response["description"]["text"] = m_Config.Motd();
			response["enforcesSecureChat"] = false;

			NetworkBuffer payload;
			payload.WriteString(response.dump());
			connection->SendRawPacket(Clientbound::Status::StatusResponse, payload);

		} else if (packetId == Serverbound::Status::PingRequest) {
			int64_t timestamp = buffer.ReadLong();

			NetworkBuffer payload;
			payload.WriteLong(timestamp);
			connection->SendRawPacket(Clientbound::Status::PongResponse, payload);
		}
	}

	void PacketHandler::HandleLogin(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer) {
		if (packetId == Serverbound::Login::Hello) {
			HandleLoginHello(connection, buffer);
		} else if (packetId == Serverbound::Login::Key) {
			HandleEncryptionResponse(connection, buffer);
		} else if (packetId == Serverbound::Login::LoginAcknowledged) {
			AX_CORE_TRACE("Login acknowledged from {}", connection->RemoteAddress());
			connection->State(ConnectionState::Configuration);
		} else {
			AX_CORE_WARN("Unknown login packet: 0x{:02X}", packetId);
		}
	}

	void PacketHandler::HandleLoginHello(Ref<Connection> connection, NetworkBuffer& buffer) {
		std::string playerName = buffer.ReadString(16);
		AX_CORE_INFO("Login request from {} ({})", playerName, connection->RemoteAddress());

		if (m_Config.OnlineMode()) {
			// Generate verify token
			std::array<uint8_t, 4> verifyToken;
			std::uniform_int_distribution<int> distribution(0, 255);
			for (auto& byte : verifyToken) {
				byte = static_cast<uint8_t>(distribution(m_Random));
			}

			{
				std::lock_guard<std::mutex> lock(m_PendingLoginsMutex);
				m_PendingLogins[connection.get()] = PendingLogin{playerName, verifyToken};
			}

			// Send Encryption Request
			NetworkBuffer payload;
			payload.WriteString("");  // Server ID (empty)
			payload.WriteVarInt(static_cast<int32_t>(m_KeyPair.PublicKeyDer().size()));
			payload.WriteBytes(m_KeyPair.PublicKeyDer());
			payload.WriteVarInt(static_cast<int32_t>(verifyToken.size()));
			payload.WriteBytes(verifyToken.data(), verifyToken.size());
			payload.WriteBoolean(true);  // Should authenticate

			connection->SendRawPacket(Clientbound::Login::Hello, payload);
		} else {
			// Offline mode — skip encryption
			// Generate offline UUID from player name
			std::string offlineUuid = "00000000-0000-3000-8000-" + playerName.substr(0, 12);
			while (offlineUuid.size() < 36) {
				offlineUuid += "0";
			}
			CompleteLogin(connection, offlineUuid, playerName);
		}
	}

	void PacketHandler::HandleEncryptionResponse(Ref<Connection> connection, NetworkBuffer& buffer) {
		// Read encrypted shared secret
		int32_t secretLength = buffer.ReadVarInt();
		auto encryptedSecret = buffer.ReadBytes(secretLength);

		// Read encrypted verify token
		int32_t tokenLength = buffer.ReadVarInt();
		auto encryptedToken = buffer.ReadBytes(tokenLength);

		// Decrypt both
		std::vector<uint8_t> sharedSecret;
		std::vector<uint8_t> verifyToken;
		try {
			sharedSecret = m_KeyPair.Decrypt(encryptedSecret);
			verifyToken = m_KeyPair.Decrypt(encryptedToken);
		} catch (const std::exception& exception) {
			AX_CORE_ERROR("Decryption failed for {}: {}", connection->RemoteAddress(), exception.what());
			connection->Disconnect("Encryption error");
			return;
		}

		// Verify token
		PendingLogin pendingLogin;
		{
			std::lock_guard<std::mutex> lock(m_PendingLoginsMutex);
			auto iterator = m_PendingLogins.find(connection.get());
			if (iterator == m_PendingLogins.end()) {
				connection->Disconnect("No pending login");
				return;
			}
			pendingLogin = std::move(iterator->second);
			m_PendingLogins.erase(iterator);
		}

		if (verifyToken.size() != pendingLogin.verifyToken.size() ||
			!std::equal(verifyToken.begin(), verifyToken.end(), pendingLogin.verifyToken.begin())) {
			connection->Disconnect("Verify token mismatch");
			return;
		}

		// Enable encryption immediately
		connection->EnableEncryption(sharedSecret);

		// Authenticate with Mojang on a separate thread
		auto connectionRef = connection;
		std::string playerName = pendingLogin.playerName;
		auto publicKey = m_KeyPair.PublicKeyDer();

		std::thread([this, connectionRef, playerName, sharedSecret, publicKey]() {
			std::string serverHash = MinecraftServerHash("", sharedSecret, publicKey);

			auto profile = MojangAuth::HasJoined(playerName, serverHash);

			if (!profile) {
				connectionRef->Disconnect("Failed to verify username");
				return;
			}

			AX_CORE_INFO("Authenticated {} (UUID: {})", profile->name, profile->uuid);
			CompleteLogin(connectionRef, FormatUuid(profile->uuid), profile->name);

		}).detach();
	}

	void PacketHandler::CompleteLogin(Ref<Connection> connection, const std::string& uuid,
		const std::string& playerName) {

		// Send Set Compression
		{
			NetworkBuffer payload;
			payload.WriteVarInt(COMPRESSION_THRESHOLD);
			connection->SendRawPacket(Clientbound::Login::LoginCompression, payload);
			connection->SetCompressionThreshold(COMPRESSION_THRESHOLD);
		}

		// Send Login Finished
		{
			NetworkBuffer payload;
			// UUID as 2 longs
			// Parse UUID string "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
			std::string cleanUuid = uuid;
			std::erase(cleanUuid, '-');

			if (cleanUuid.size() == 32) {
				uint64_t mostSignificant = 0;
				uint64_t leastSignificant = 0;
				for (int i = 0; i < 16; i++) {
					char hexByte[3] = {cleanUuid[i * 2], cleanUuid[i * 2 + 1], '\0'};
					uint8_t byte = static_cast<uint8_t>(std::strtoul(hexByte, nullptr, 16));
					if (i < 8) {
						mostSignificant = (mostSignificant << 8) | byte;
					} else {
						leastSignificant = (leastSignificant << 8) | byte;
					}
				}
				payload.WriteLong(static_cast<int64_t>(mostSignificant));
				payload.WriteLong(static_cast<int64_t>(leastSignificant));
			} else {
				payload.WriteLong(0);
				payload.WriteLong(0);
			}

			payload.WriteString(playerName);

			// Properties (empty for now)
			payload.WriteVarInt(0);

			connection->SendRawPacket(Clientbound::Login::LoginFinished, payload);
		}

		AX_CORE_INFO("{} has logged in [{}]", playerName, uuid);
	}

	std::string PacketHandler::FormatUuid(const std::string& trimmedUuid) const {
		if (trimmedUuid.size() != 32) {
			return trimmedUuid;
		}
		return trimmedUuid.substr(0, 8) + "-"
			+ trimmedUuid.substr(8, 4) + "-"
			+ trimmedUuid.substr(12, 4) + "-"
			+ trimmedUuid.substr(16, 4) + "-"
			+ trimmedUuid.substr(20, 12);
	}

}
