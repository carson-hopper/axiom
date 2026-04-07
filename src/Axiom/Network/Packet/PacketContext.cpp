#include "PacketContext.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Protocol.h"

namespace Axiom {

	static constexpr int32_t COMPRESSION_THRESHOLD = 256;

	PacketContext::PacketContext(ServerConfig& config, EventBus& eventBus, CommandRegistry& commands)
		: m_Config(config)
		, m_EventBus(eventBus)
		, m_Commands(commands) {

		m_Registries.LoadAll("data");
	}

	void PacketContext::StorePendingLogin(Connection* connection, PendingLogin login) {
		std::lock_guard<std::mutex> lock(m_PendingLoginsMutex);
		m_PendingLogins[connection] = std::move(login);
	}

	std::optional<PendingLogin> PacketContext::TakePendingLogin(Connection* connection) {
		std::lock_guard<std::mutex> lock(m_PendingLoginsMutex);
		auto iterator = m_PendingLogins.find(connection);
		if (iterator == m_PendingLogins.end()) {
			return std::nullopt;
		}
		auto login = std::move(iterator->second);
		m_PendingLogins.erase(iterator);
		return login;
	}

	std::array<uint8_t, 4> PacketContext::GenerateVerifyToken() {
		std::array<uint8_t, 4> token;
		std::uniform_int_distribution<int> distribution(0, 255);
		for (auto& byte : token) {
			byte = static_cast<uint8_t>(distribution(m_Random));
		}
		return token;
	}

	std::string PacketContext::FormatUuid(const std::string& trimmedUuid) const {
		if (trimmedUuid.size() != 32) {
			return trimmedUuid;
		}
		return trimmedUuid.substr(0, 8) + "-"
			+ trimmedUuid.substr(8, 4) + "-"
			+ trimmedUuid.substr(12, 4) + "-"
			+ trimmedUuid.substr(16, 4) + "-"
			+ trimmedUuid.substr(20, 12);
	}

	void PacketContext::CompleteLogin(Ref<Connection> connection, const std::string& uuid,
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
			payload.WriteVarInt(0); // Properties count

			connection->SendRawPacket(Clientbound::Login::LoginFinished, payload);
		}

		AX_CORE_INFO("{} has logged in [{}]", playerName, uuid);
	}

}
