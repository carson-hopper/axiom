#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Identifier.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Network/Crypto/AesCipher.h"
#include "Axiom/Network/Packet/Packet.h"

#include <asio.hpp>

#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

namespace Axiom {

	struct ConnectionIdTag {};
	using ConnectionId = Identifier<ConnectionIdTag>;

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
		~Connection() override;

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

		void SendRawPacket(IChainablePacket& packet);
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
		void ProtocolVersion(const int32_t version) { m_ProtocolVersion.store(version, std::memory_order_release); }

		int8_t RequestedViewDistance() const {
			return m_RequestedViewDistance.load(std::memory_order_acquire);
		}

		void RequestedViewDistance(const int8_t distance) {
			m_RequestedViewDistance.store(distance, std::memory_order_release);
		}

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
		/**
		 * Build a Ref<Connection> to this object. The
		 * intrusive refcount on RefCounted means calling
		 * Ref<Connection>(this) is a legitimate way to
		 * extend the object's lifetime from inside any
		 * method — it increments the existing counter
		 * rather than creating an independent control
		 * block. This helper documents that intent at
		 * every call site (async handler captures, posted
		 * lambdas, callbacks invoked with a Ref argument)
		 * and keeps the idiom searchable in one place.
		 *
		 * Requires: m_RefCount > 0. Calling Self() from
		 * the destructor or before Start() is UB — by
		 * then no external owner is keeping us alive and
		 * the resurrected Ref would dangle.
		 */
		Ref<Connection> Self() { return Ref<Connection>(this); }

		void ReadLoop();
		void ReadFrameLength(std::vector<uint8_t> preDecrypted = {});
		void ReadVarIntChunk(Ref<Connection> self, std::shared_ptr<std::vector<uint8_t>> accumulatedBytes);
		void ReadFrameBody(int32_t frameLength, std::vector<uint8_t> leftover = {});
		void ProcessPacket(std::vector<uint8_t> data);

		/**
		 * Arm or re-arm the frame read deadline. Called at
		 * the start of every ReadFrameLength so a slow or
		 * trickling client cannot hold the connection past
		 * FrameReadTimeout without making progress.
		 */
		void ArmReadTimeout();

		/**
		 * Pop the next frame off m_WriteQueue and issue an
		 * async_write for it. Called from SendRawPacket when
		 * no write is in flight, and from the write handler
		 * when a previous write completes. Holds m_WriteMutex
		 * only while popping the queue, not during the
		 * actual I/O.
		 */
		void StartNextWrite();

		std::vector<uint8_t> CompressPacket(int32_t packetId, const NetworkBuffer& payload, int32_t threshold);

		// Invokes the packet handler with proper synchronization
		void InvokePacketHandler(int32_t packetId, NetworkBuffer& buffer);

		/**
		 * Maximum wall-clock time a single frame may take
		 * to arrive in full (varint length + body). Matches
		 * the industry default used by Netty, Paper, and
		 * Velocity for Minecraft server read timeouts.
		 */
		static constexpr auto FrameReadTimeout = std::chrono::seconds(30);

		/**
		 * Cap on the number of frames queued for write but
		 * not yet sent. A slow or hung client past this
		 * limit is force-disconnected rather than letting
		 * the per-connection memory grow unbounded. Must be
		 * large enough to absorb the initial chunk-data
		 * burst on world join (view distance 10 ≈ 441
		 * chunks).
		 */
		static constexpr size_t MaxQueuedWriteFrames = 2048;

		ConnectionId m_Id;
		asio::ip::tcp::socket m_Socket;
		asio::steady_timer m_ReadTimer{m_Socket.get_executor()};

		// Atomic state variables for thread-safe access
		std::atomic<ConnectionState> m_State{ConnectionState::Handshake};
		std::atomic<int32_t> m_ProtocolVersion{0};
		std::atomic<bool> m_Connected{false};
		std::atomic<int32_t> m_CompressionThreshold{-1};
		// -1 = client has not yet sent a ClientInformation packet.
		// Otherwise the client's requested render distance in chunks,
		// clamped to the server maximum at SendInitialChunks time.
		std::atomic<int8_t> m_RequestedViewDistance{-1};

		// Handler mutex protects callback modification
		mutable std::shared_mutex m_HandlerMutex;
		PacketHandler m_PacketHandler;

		// Write mutex protects m_Cipher, m_WriteQueue, and
		// m_WriteInFlight. The actual async_write to the
		// socket runs *without* the mutex held so a slow
		// client cannot stall other writers or the read
		// path (which shares the cipher under the same mutex).
		std::mutex m_WriteMutex;
		Scope<AesCipher> m_Cipher;
		std::deque<std::vector<uint8_t>> m_WriteQueue;
		bool m_WriteInFlight = false;
	};

}
