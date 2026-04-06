#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Network/Crypto/AesCipher.h"

#include <asio.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace Axiom {

	class Connection : public std::enable_shared_from_this<Connection> {
	public:
		using PacketHandler = std::function<void(Ref<Connection>, int32_t, NetworkBuffer&)>;

		explicit Connection(asio::ip::tcp::socket socket);
		~Connection();

		Connection(const Connection&) = delete;
		Connection& operator=(const Connection&) = delete;

		void Start();
		void Disconnect(const std::string& reason = "");
		void SendRawPacket(int32_t packetId, const NetworkBuffer& payload);

		ConnectionState State() const { return m_State; }
		void State(ConnectionState state) { m_State = state; }

		int32_t ProtocolVersion() const { return m_ProtocolVersion; }
		void ProtocolVersion(int32_t version) { m_ProtocolVersion = version; }

		void SetPacketHandler(PacketHandler handler) { m_PacketHandler = std::move(handler); }

		void EnableEncryption(const std::vector<uint8_t>& sharedSecret);
		void SetCompressionThreshold(int32_t threshold);

		std::string RemoteAddress() const;
		bool IsConnected() const { return m_Connected; }

	private:
		void ReadLoop();
		void ReadFrameLength();
		void ReadFrameBody(int32_t frameLength);
		void ProcessPacket(std::vector<uint8_t> data);

		std::vector<uint8_t> CompressPacket(int32_t packetId, const NetworkBuffer& payload);

		asio::ip::tcp::socket m_Socket;
		ConnectionState m_State = ConnectionState::Handshake;
		int32_t m_ProtocolVersion = 0;
		PacketHandler m_PacketHandler;
		bool m_Connected = false;
		std::mutex m_WriteMutex;

		Scope<AesCipher> m_Cipher;
		int32_t m_CompressionThreshold = -1;
	};

}
