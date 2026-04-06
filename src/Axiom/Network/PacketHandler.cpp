#include "PacketHandler.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Config/ServerConfig.h"

#include <nlohmann/json.hpp>

namespace Axiom {

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
			// Build status response JSON
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

		} else {
			AX_CORE_WARN("Unknown status packet: 0x{:02X}", packetId);
		}
	}

	void PacketHandler::HandleLogin(Ref<Connection> connection, int32_t packetId, NetworkBuffer& buffer) {
		if (packetId == Serverbound::Login::Hello) {
			std::string playerName = buffer.ReadString(16);
			AX_CORE_INFO("Login request from {} ({})", playerName, connection->RemoteAddress());

			// For now, just disconnect with a message
			nlohmann::json disconnectMessage;
			disconnectMessage["text"] = "Server is still under construction!";
			disconnectMessage["color"] = "gold";

			NetworkBuffer payload;
			payload.WriteString(disconnectMessage.dump());
			connection->SendRawPacket(Clientbound::Login::Disconnect, payload);

			connection->Disconnect();
		} else {
			AX_CORE_WARN("Unknown login packet: 0x{:02X}", packetId);
		}
	}

}
