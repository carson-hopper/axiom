#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Network/Crypto/ServerKeyPair.h"

#include <array>
#include <cstdint>
#include <mutex>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	class ServerConfig;

	struct PendingLogin {
		std::string playerName;
		std::array<uint8_t, 4> verifyToken;
	};

	class PacketHandler {
	public:
		explicit PacketHandler(ServerConfig& config);

		void HandlePacket(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer);

		const ServerKeyPair& KeyPair() const { return m_KeyPair; }

	private:
		void HandleHandshake(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer);
		void HandleStatus(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer);
		void HandleLogin(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer);

		void HandleLoginHello(Ref<Connection> connection, NetworkBuffer& buffer);
		void HandleEncryptionResponse(Ref<Connection> connection, NetworkBuffer& buffer);

		void CompleteLogin(Ref<Connection> connection, const std::string& uuid,
			const std::string& playerName);

		std::string FormatUuid(const std::string& trimmedUuid) const;

		ServerConfig& m_Config;
		ServerKeyPair m_KeyPair;

		std::mutex m_PendingLoginsMutex;
		std::unordered_map<Connection*, PendingLogin> m_PendingLogins;

		std::mt19937 m_Random{std::random_device{}()};
	};

}
