#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Network/Crypto/AesCipher.h"

#include <asio.hpp>

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Represents a TCP connection to a Minecraft client.
	 *
	 * Handles packet framing, encryption, compression, and async I/O.
	 * This class is thread-safe for concurrent read/write operations.
	 *
	 * Thread safety notes:
	 * - State/ProtocolVersion accessors are atomic
	 * - SendRawPacket is thread-safe (uses write mutex)
	 * - PacketHandler is invoked from ASIO worker threads
	 * - Disconnect is thread-safe and idempotent
	 */
	class Connection : public virtual RefCounted {
	public:
		using PacketHandler = std::function<void(Ref<Connection>, int32_t, NetworkBuffer&)>;

		explicit Connection(asio::ip::tcp::socket socket);
		~Connection();

		Connection(const Connection&) = delete;
		Connection& operator=(const Connection&) = delete;

		/**
		 * Starts the connection read loop.
		 *
		 * Must be called once after construction. Initiates async read
		 * operations on the socket.
		 */
		void Start();

		/**
		 * Gracefully disconnects the connection.
		 *
		 * Thread-safe. Can be called from any thread. Idempotent -
		 * subsequent calls have no effect.
		 *
		 * @param reason Reason for disconnection (logged if non-empty).
		 */
		void Disconnect(const std::string& reason = "");

		/**
		 * Sends a raw packet to the client.
		 *
		 * Thread-safe. Can be called from any thread. If the connection
		 * is not connected, the packet is silently dropped.
		 *
		 * @param packetId The packet ID to send.
		 * @param payload The packet payload data.
		 */
		void SendRawPacket(int32_t packetId, const NetworkBuffer& payload);

		/**
		 * Gets the current connection state.
		 *
		 * Thread-safe. Uses atomic operations.
		 */
		ConnectionState State() const { return m_State.load(std::memory_order_acquire); }

		/**
		 * Sets the connection state.
		 *
		 * Thread-safe. Uses atomic operations.
		 */
		void State(ConnectionState state) { m_State.store(state, std::memory_order_release); }

		/**
		 * Gets the protocol version negotiated during handshake.
		 *
		 * Thread-safe. Uses atomic operations.
		 */
		int32_t ProtocolVersion() const { return m_ProtocolVersion.load(std::memory_order_acquire); }

		/**
		 * Sets the protocol version.
		 *
		 * Thread-safe. Uses atomic operations.
		 */
		void ProtocolVersion(int32_t version) { m_ProtocolVersion.store(version, std::memory_order_release); }

		/**
		 * Sets the packet handler callback.
		 *
		 * Thread-safe. Uses mutex protection. The handler will be invoked
		 * from ASIO worker threads when packets are received.
		 */
		void SetPacketHandler(PacketHandler handler);

		/**
		 * Enables AES encryption for this connection.
		 *
		 * Thread-safe. Uses mutex protection. All subsequent packets
		 * will be encrypted/decrypted.
		 *
		 * @param sharedSecret The shared secret from key exchange.
		 */
		void EnableEncryption(const std::vector<uint8_t>& sharedSecret);

		/**
		 * Sets the compression threshold.
		 *
		 * Thread-safe. Uses mutex protection.
		 *
		 * @param threshold Packets larger than this (in bytes) will be compressed.
		 *                  -1 disables compression.
		 */
		void SetCompressionThreshold(int32_t threshold);

		/**
		 * Gets the unique connection ID.
		 *
		 * Thread-safe. ID is immutable after construction.
		 */
		ConnectionId Id() const { return m_Id; }

		/**
		 * Gets the remote endpoint address as a string.
		 *
		 * Thread-safe. Returns "unknown" if the endpoint is unavailable.
		 */
		std::string RemoteAddress() const;

		/**
		 * Checks if the connection is currently connected.
		 *
		 * Thread-safe. Uses atomic operations.
		 */
		bool IsConnected() const { return m_Connected.load(std::memory_order_acquire); }

	private:
		void ReadLoop();
		void ReadFrameLength();
		void ReadFrameBody(int32_t frameLength);
		void ProcessPacket(std::vector<uint8_t> data);

		std::vector<uint8_t> CompressPacket(int32_t packetId, const NetworkBuffer& payload, int32_t threshold);

		// Invokes the packet handler with proper synchronization
		void InvokePacketHandler(int32_t packetId, NetworkBuffer& buffer);

	private:
		ConnectionId m_Id;
		asio::ip::tcp::socket m_Socket;

		// Atomic state variables for thread-safe access
		std::atomic<ConnectionState> m_State{ConnectionState::Handshake};
		std::atomic<int32_t> m_ProtocolVersion{0};
		std::atomic<bool> m_Connected{false};
		std::atomic<int32_t> m_CompressionThreshold{-1};

		// Handler mutex protects callback modification
		mutable std::shared_mutex m_HandlerMutex;
		PacketHandler m_PacketHandler;

		// Write mutex protects socket writes and cipher operations
		std::mutex m_WriteMutex;
		Scope<AesCipher> m_Cipher;
	};

}
