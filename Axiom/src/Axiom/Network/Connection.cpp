#include "Connection.h"

#include "Axiom/Core/Log.h"

#include <zlib.h>

namespace Axiom {

	Connection::Connection(asio::ip::tcp::socket socket)
		: m_Socket(std::move(socket)) {}

	Connection::~Connection() {
		m_Connected.store(false, std::memory_order_release);

		try {
			m_ReadTimer.cancel();
		} catch (const std::exception& exception) {
			AX_CORE_TRACE("Read timer cancel error: {}", exception.what());
		}

		asio::error_code errorCode;
		m_Socket.shutdown(asio::ip::tcp::socket::shutdown_both, errorCode);
		if (errorCode && errorCode != asio::error::not_connected) {
			AX_CORE_TRACE("Socket shutdown error in destructor: {}", errorCode.message());
		}
		m_Socket.close(errorCode);
	}

	void Connection::Start() {
		m_Connected.store(true, std::memory_order_release);
		AX_CORE_TRACE("Connection opened from {}", RemoteAddress());
		ReadLoop();
	}

	void Connection::Disconnect(const std::string& reason) {
		bool expected = true;
		if (!m_Connected.compare_exchange_strong(expected, false, std::memory_order_acq_rel)) {
			return;  // Already disconnected
		}

		if (!reason.empty()) {
			AX_CORE_INFO("Disconnecting {}: {}", RemoteAddress(), reason);
		}

		try {
			m_ReadTimer.cancel();
		} catch (const std::exception& exception) {
			AX_CORE_TRACE("Read timer cancel error: {}", exception.what());
		}

		{
			std::lock_guard<std::mutex> const lock(m_WriteMutex);
			m_WriteQueue.clear();
		}

		auto self = Self();
		asio::post(m_Socket.get_executor(), [self]() {
			asio::error_code errorCode;
			self->m_Socket.shutdown(asio::ip::tcp::socket::shutdown_both, errorCode);
			if (errorCode && errorCode != asio::error::not_connected) {
				AX_CORE_TRACE("Socket shutdown error: {}", errorCode.message());
			}
			self->m_Socket.close(errorCode);
		});
	}

	void Connection::SendRawPacket(const int32_t packetId, const NetworkBuffer& payload) {
		if (!m_Connected.load(std::memory_order_acquire)) {
			return;
		}

		std::vector<uint8_t> frameData;
		const int32_t compressionThreshold = m_CompressionThreshold.load(std::memory_order_acquire);

		if (compressionThreshold >= 0) {
			frameData = CompressPacket(packetId, payload, compressionThreshold);
		} else {
			NetworkBuffer packet;
			packet.WriteVarInt(packetId);
			packet.WriteBytes(payload.Data());

			NetworkBuffer frame;
			frame.WriteVarInt(static_cast<int32_t>(packet.Size()));
			frame.WriteBytes(packet.Data());
			frameData = std::move(frame.Data());
		}

		// Encryption and queue insertion happen under the
		// mutex so that: (a) the cipher state advances in
		// the same order frames enter the queue, (b) the
		// read path's decrypt step sees a consistent cipher
		// snapshot. The actual socket write is issued
		// WITHOUT the mutex held (see StartNextWrite) so
		// that a slow client cannot stall other writers
		// or the read loop.
		bool shouldStartWrite = false;
		bool overflow = false;
		{
			std::lock_guard<std::mutex> const lock(m_WriteMutex);

			if (m_Cipher) {
				frameData = m_Cipher->Encrypt(frameData);
			}

			if (m_WriteQueue.size() >= MaxQueuedWriteFrames) {
				AX_CORE_WARN("Write queue overflow for {} ({} frames); disconnecting",
					RemoteAddress(), m_WriteQueue.size());
				overflow = true;
			} else {
				m_WriteQueue.push_back(std::move(frameData));

				if (!m_WriteInFlight) {
					m_WriteInFlight = true;
					shouldStartWrite = true;
				}
			}
		}

		if (shouldStartWrite) {
			StartNextWrite();
		} else if (overflow) {
			// We failed to enqueue because the queue was
			// already at its cap. Force-disconnect the
			// client (its buffer is too far behind). Must
			// not read m_WriteInFlight here — that would
			// race with a concurrent StartNextWrite drain.
			Disconnect("Write queue overflow");
		}
	}

	void Connection::StartNextWrite() {
		// Pop the next frame from the queue (still under
		// the mutex) but issue the async_write outside so
		// readers and other writers are not blocked by the
		// duration of a single socket write.
		auto bufferToSend = std::make_shared<std::vector<uint8_t>>();
		{
			std::lock_guard<std::mutex> const lock(m_WriteMutex);
			if (m_WriteQueue.empty()) {
				m_WriteInFlight = false;
				return;
			}
			*bufferToSend = std::move(m_WriteQueue.front());
			m_WriteQueue.pop_front();
		}

		auto self = Self();
		asio::async_write(m_Socket, asio::buffer(*bufferToSend),
			[this, self, bufferToSend](const asio::error_code& errorCode, size_t /*bytesWritten*/) {
				if (errorCode) {
					if (errorCode != asio::error::eof && errorCode != asio::error::operation_aborted) {
						AX_CORE_TRACE("Write error to {}: {}", RemoteAddress(), errorCode.message());
					}
					Disconnect();
					return;
				}

				// Chain: dispatch the next queued frame
				// if any. StartNextWrite clears the
				// in-flight flag if the queue is now empty.
				StartNextWrite();
			});
	}

	void Connection::SetPacketHandler(PacketHandler handler) {
		std::unique_lock lock(m_HandlerMutex);
		m_PacketHandler = std::move(handler);
	}

	void Connection::EnableEncryption(const std::vector<uint8_t>& sharedSecret) {
		std::lock_guard lock(m_WriteMutex);
		m_Cipher = CreateScope<AesCipher>(sharedSecret);
		AX_CORE_TRACE("Encryption enabled for {}", RemoteAddress());
	}

	void Connection::SetCompressionThreshold(int32_t threshold) {
		m_CompressionThreshold.store(threshold, std::memory_order_release);
		AX_CORE_TRACE("Compression threshold set to {} for {}", threshold, RemoteAddress());
	}

	std::string Connection::RemoteAddress() const {
		try {
			const auto endpoint = m_Socket.remote_endpoint();
			return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
		} catch (...) {
			return "unknown";
		}
	}

	void Connection::ReadLoop() {
		ReadFrameLength();
	}

	static constexpr size_t MaxVarIntBytes = 5;

	void Connection::ArmReadTimeout() {
		m_ReadTimer.expires_after(FrameReadTimeout);

		auto self = Self();
		m_ReadTimer.async_wait([this, self](const asio::error_code& errorCode) {
			if (errorCode == asio::error::operation_aborted) {
				return;
			}
			if (!m_Connected.load(std::memory_order_acquire)) {
				return;
			}
			AX_CORE_TRACE("Frame read timeout from {}", RemoteAddress());
			Disconnect("Frame read timeout");
		});
	}

	void Connection::ReadFrameLength(std::vector<uint8_t> preDecrypted) {
		if (!m_Connected.load(std::memory_order_acquire)) {
			return;
		}

		ArmReadTimeout();

		auto self = Self();
		auto accumulatedBytes = std::make_shared<std::vector<uint8_t>>();
		accumulatedBytes->reserve(MaxVarIntBytes);

		for (size_t byteIndex = 0; byteIndex < preDecrypted.size(); byteIndex++) {
			const uint8_t decryptedByte = preDecrypted[byteIndex];
			accumulatedBytes->push_back(decryptedByte);

			if ((decryptedByte & 0x80) != 0) {
				if (accumulatedBytes->size() >= MaxVarIntBytes) {
					Disconnect("VarInt too long");
					return;
				}
				continue;
			}

			NetworkBuffer parser(*accumulatedBytes);
			int32_t frameLength = 0;
			try {
				frameLength = parser.ReadVarInt();
			} catch (...) {
				Disconnect("VarInt parse failed");
				return;
			}

			if (frameLength <= 0 || frameLength > 0x200000) {
				Disconnect("Invalid frame length");
				return;
			}

			std::vector<uint8_t> bodyLeftover(
				preDecrypted.begin() + byteIndex + 1,
				preDecrypted.end());
			ReadFrameBody(frameLength, std::move(bodyLeftover));
			return;
		}

		ReadVarIntChunk(std::move(self), std::move(accumulatedBytes));
	}

	void Connection::ReadVarIntChunk(Ref<Connection> self,
		std::shared_ptr<std::vector<uint8_t>> accumulatedBytes) {

		const size_t alreadyHave = accumulatedBytes->size();
		const size_t slotsLeft = MaxVarIntBytes - alreadyHave;

		auto chunk = std::make_shared<std::vector<uint8_t>>(slotsLeft);

		asio::async_read(m_Socket, asio::buffer(*chunk),
			asio::transfer_at_least(1),
			[this, self, accumulatedBytes, chunk](
				const asio::error_code& errorCode, size_t bytesTransferred) {

				if (errorCode) {
					if (errorCode != asio::error::eof && errorCode != asio::error::operation_aborted) {
						AX_CORE_TRACE("Read error from {}: {}", RemoteAddress(), errorCode.message());
					}
					Disconnect();
					return;
				}

				std::vector<uint8_t> newlyDecrypted(chunk->begin(), chunk->begin() + bytesTransferred);
				{
					std::lock_guard<std::mutex> const lock(m_WriteMutex);
					if (m_Cipher) {
						newlyDecrypted = m_Cipher->Decrypt(newlyDecrypted);
						if (newlyDecrypted.size() != bytesTransferred) {
							Disconnect("Decryption failed");
							return;
						}
					}
				}

				for (size_t byteIndex = 0; byteIndex < newlyDecrypted.size(); byteIndex++) {
					const uint8_t decryptedByte = newlyDecrypted[byteIndex];
					accumulatedBytes->push_back(decryptedByte);

					if ((decryptedByte & 0x80) != 0) {
						if (accumulatedBytes->size() >= MaxVarIntBytes) {
							Disconnect("VarInt too long");
							return;
						}
						continue;
					}

					NetworkBuffer parser(*accumulatedBytes);
					int32_t frameLength = 0;
					try {
						frameLength = parser.ReadVarInt();
					} catch (...) {
						Disconnect("VarInt parse failed");
						return;
					}

					if (frameLength <= 0 || frameLength > 0x200000) {
						Disconnect("Invalid frame length");
						return;
					}

					std::vector<uint8_t> bodyLeftover(
						newlyDecrypted.begin() + byteIndex + 1,
						newlyDecrypted.end());
					ReadFrameBody(frameLength, std::move(bodyLeftover));
					return;
				}

				if (accumulatedBytes->size() >= MaxVarIntBytes) {
					Disconnect("VarInt too long");
					return;
				}
				ReadVarIntChunk(self, accumulatedBytes);
			});
	}

	void Connection::ReadFrameBody(int32_t frameLength, std::vector<uint8_t> leftover) {
		if (leftover.size() >= static_cast<size_t>(frameLength)) {
			std::vector<uint8_t> data(leftover.begin(), leftover.begin() + frameLength);

			std::vector<uint8_t> nextFrameCarryover(
				leftover.begin() + static_cast<ptrdiff_t>(frameLength),
				leftover.end());

			ProcessPacket(std::move(data));

			if (m_Connected.load(std::memory_order_acquire)) {
				ReadFrameLength(std::move(nextFrameCarryover));
			}
			return;
		}

		auto self = Self();
		auto bodyBuffer = std::make_shared<std::vector<uint8_t>>(frameLength);
		std::ranges::copy(leftover, bodyBuffer->begin());
		const size_t leftoverSize = leftover.size();
		const size_t remaining = static_cast<size_t>(frameLength) - leftoverSize;

		asio::async_read(m_Socket,
			asio::buffer(bodyBuffer->data() + leftoverSize, remaining),
			[this, self, bodyBuffer, leftoverSize, remaining](
				const asio::error_code& errorCode, size_t /*bytesTransferred*/) {

				if (errorCode) {
					if (errorCode != asio::error::eof && errorCode != asio::error::operation_aborted) {
						AX_CORE_TRACE("Read error from {}: {}", RemoteAddress(), errorCode.message());
					}
					Disconnect();
					return;
				}

				{
					std::lock_guard<std::mutex> const lock(m_WriteMutex);
					if (m_Cipher) {
						const std::vector<uint8_t> encryptedTail(
							bodyBuffer->begin() + leftoverSize,
							bodyBuffer->end());
						auto decryptedTail = m_Cipher->Decrypt(encryptedTail);
						if (decryptedTail.size() != remaining) {
							Disconnect("Decryption failed");
							return;
						}
						std::ranges::copy(decryptedTail,
							bodyBuffer->begin() + leftoverSize);
					}
				}

				ProcessPacket(std::move(*bodyBuffer));

				if (m_Connected.load(std::memory_order_acquire)) {
					ReadFrameLength();
				}
			});
	}

	void Connection::ProcessPacket(std::vector<uint8_t> data) {
		static constexpr int32_t MaxUncompressedPacketSize = 8 * 1024 * 1024;

		try {
			const int32_t compressionThreshold = m_CompressionThreshold.load(std::memory_order_acquire);

			if (compressionThreshold >= 0) {
				NetworkBuffer compressedBuffer(std::move(data));
				const int32_t dataLength = compressedBuffer.ReadVarInt();

				if (dataLength == 0) {
					auto remaining = compressedBuffer.ReadRemainingBytes();
					NetworkBuffer buffer(std::move(remaining));
					const int32_t packetId = buffer.ReadVarInt();
					InvokePacketHandler(packetId, buffer);
				} else {
					if (dataLength < compressionThreshold) {
						Disconnect("Decompressed size below compression threshold");
						return;
					}
					if (dataLength > MaxUncompressedPacketSize) {
						Disconnect("Decompressed size exceeds maximum");
						return;
					}

					const auto compressedData = compressedBuffer.ReadRemainingBytes();
					std::vector<uint8_t> decompressed(static_cast<size_t>(dataLength));
					uLongf decompressedSize = static_cast<uLongf>(dataLength);

					const int result = uncompress(decompressed.data(), &decompressedSize,
						compressedData.data(), static_cast<uLong>(compressedData.size()));

					if (result != Z_OK) {
						Disconnect("Decompression failed");
						return;
					}

					if (static_cast<int32_t>(decompressedSize) != dataLength) {
						Disconnect("Decompressed size mismatch");
						return;
					}

					NetworkBuffer buffer(std::move(decompressed));
					const int32_t packetId = buffer.ReadVarInt();
					InvokePacketHandler(packetId, buffer);
				}
			} else {
				NetworkBuffer buffer(std::move(data));
				const int32_t packetId = buffer.ReadVarInt();
				InvokePacketHandler(packetId, buffer);
			}
		} catch (const std::exception& exception) {
			AX_CORE_WARN("Malformed packet from {}: {}", RemoteAddress(), exception.what());
			Disconnect("Malformed packet");
		} catch (...) {
			AX_CORE_WARN("Unknown exception processing packet from {}", RemoteAddress());
			Disconnect("Malformed packet");
		}
	}

	std::vector<uint8_t> Connection::CompressPacket(const int32_t packetId, const NetworkBuffer& payload, const int32_t threshold) {
		NetworkBuffer uncompressedBody;
		uncompressedBody.WriteVarInt(packetId);
		uncompressedBody.WriteBytes(payload.Data());

		const int32_t dataLength = static_cast<int32_t>(uncompressedBody.Size());

		if (dataLength < threshold) {
			NetworkBuffer frame;
			const int32_t packetLength = NetworkBuffer::VarIntSize(0) + dataLength;
			frame.WriteVarInt(packetLength);
			frame.WriteVarInt(0);
			frame.WriteBytes(uncompressedBody.Data());
			return std::move(frame.Data());
		}

		uLongf compressedSize = compressBound(static_cast<uLong>(uncompressedBody.Size()));
		std::vector<uint8_t> compressed(compressedSize);

		int result = compress(compressed.data(), &compressedSize,
			uncompressedBody.Data().data(), static_cast<uLong>(uncompressedBody.Size()));

		if (result != Z_OK) {
			AX_CORE_ERROR("Compression failed with code {}", result);
			return {};
		}
		compressed.resize(compressedSize);

		NetworkBuffer frame;
		const int32_t packetLength = NetworkBuffer::VarIntSize(dataLength) + static_cast<int32_t>(compressedSize);
		frame.WriteVarInt(packetLength);
		frame.WriteVarInt(dataLength);
		frame.WriteBytes(compressed);
		return std::move(frame.Data());
	}

	void Connection::InvokePacketHandler(const int32_t packetId, NetworkBuffer& buffer) {
		std::shared_lock lock(m_HandlerMutex);
		if (m_PacketHandler) {
			const auto handler = m_PacketHandler;
			lock.unlock();
			handler(Self(), packetId, buffer);
		}
	}

}
