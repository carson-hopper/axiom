#include "Connection.h"

#include "Axiom/Core/Log.h"

#include <zlib.h>

namespace Axiom {

	Connection::Connection(asio::ip::tcp::socket socket)
		: m_Socket(std::move(socket)) {}

	Connection::~Connection() {
		Disconnect();
	}

	void Connection::Start() {
		m_Connected.store(true, std::memory_order_release);
		AX_CORE_TRACE("Connection opened from {}", RemoteAddress());
		ReadLoop();
	}

	void Connection::Disconnect(const std::string& reason) {
		// Use exchange to ensure we only disconnect once
		bool expected = true;
		if (!m_Connected.compare_exchange_strong(expected, false, std::memory_order_acq_rel)) {
			return;  // Already disconnected
		}

		if (!reason.empty()) {
			AX_CORE_INFO("Disconnecting {}: {}", RemoteAddress(), reason);
		}

		// Close socket with proper error handling
		asio::error_code errorCode;
		m_Socket.shutdown(asio::ip::tcp::socket::shutdown_both, errorCode);
		if (errorCode && errorCode != asio::error::not_connected) {
			AX_CORE_TRACE("Socket shutdown error: {}", errorCode.message());
		}
		m_Socket.close(errorCode);
	}

	void Connection::EnableEncryption(const std::vector<uint8_t>& sharedSecret) {
		std::lock_guard<std::mutex> lock(m_WriteMutex);
		m_Cipher = CreateScope<AesCipher>(sharedSecret);
		AX_CORE_TRACE("Encryption enabled for {}", RemoteAddress());
	}

	void Connection::SetCompressionThreshold(int32_t threshold) {
		m_CompressionThreshold.store(threshold, std::memory_order_release);
		AX_CORE_TRACE("Compression threshold set to {} for {}", threshold, RemoteAddress());
	}

	void Connection::SetPacketHandler(PacketHandler handler) {
		std::unique_lock lock(m_HandlerMutex);
		m_PacketHandler = std::move(handler);
	}

	void Connection::InvokePacketHandler(int32_t packetId, NetworkBuffer& buffer) {
		std::shared_lock lock(m_HandlerMutex);
		if (m_PacketHandler) {
			// Unlock during handler invocation to avoid deadlock
			// if handler tries to modify the handler
			auto handler = m_PacketHandler;
			lock.unlock();
			handler(shared_from_this(), packetId, buffer);
		}
	}

	void Connection::SendRawPacket(int32_t packetId, const NetworkBuffer& payload) {
		if (!m_Connected.load(std::memory_order_acquire)) {
			return;
		}

		std::vector<uint8_t> frameData;
		int32_t compressionThreshold = m_CompressionThreshold.load(std::memory_order_acquire);

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

		// Lock covers encryption + write: cipher is stateful and packets
		// must arrive in the order they were encrypted
		std::lock_guard<std::mutex> lock(m_WriteMutex);

		if (m_Cipher) {
			frameData = m_Cipher->Encrypt(frameData);
		}

		asio::error_code errorCode;
		asio::write(m_Socket, asio::buffer(frameData), errorCode);

		if (errorCode) {
			AX_CORE_TRACE("Write error to {}: {}", RemoteAddress(), errorCode.message());
			Disconnect();
		}
	}

	std::vector<uint8_t> Connection::CompressPacket(int32_t packetId, const NetworkBuffer& payload, int32_t threshold) {
		NetworkBuffer uncompressedBody;
		uncompressedBody.WriteVarInt(packetId);
		uncompressedBody.WriteBytes(payload.Data());

		int32_t dataLength = static_cast<int32_t>(uncompressedBody.Size());

		if (dataLength < threshold) {
			// Below threshold — send uncompressed with dataLength=0
			NetworkBuffer frame;
			int32_t packetLength = NetworkBuffer::VarIntSize(0) + dataLength;
			frame.WriteVarInt(packetLength);
			frame.WriteVarInt(0);
			frame.WriteBytes(uncompressedBody.Data());
			return std::move(frame.Data());
		}

		// Compress with zlib
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
		int32_t packetLength = NetworkBuffer::VarIntSize(dataLength) + static_cast<int32_t>(compressedSize);
		frame.WriteVarInt(packetLength);
		frame.WriteVarInt(dataLength);
		frame.WriteBytes(compressed);
		return std::move(frame.Data());
	}

	std::string Connection::RemoteAddress() const {
		try {
			auto endpoint = m_Socket.remote_endpoint();
			return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
		} catch (...) {
			return "unknown";
		}
	}

	void Connection::ReadLoop() {
		ReadFrameLength();
	}

	void Connection::ReadFrameLength() {
		// Check if still connected before starting async read
		if (!m_Connected.load(std::memory_order_acquire)) {
			return;
		}

		auto self = shared_from_this();
		auto lengthBuffer = std::make_shared<std::vector<uint8_t>>();

		auto singleByte = std::make_shared<uint8_t>(0);
		auto readNextByte = std::make_shared<std::function<void()>>();

		*readNextByte = [this, self, lengthBuffer, singleByte, readNextByte]() {
			asio::async_read(m_Socket, asio::buffer(singleByte.get(), 1),
				[this, self, lengthBuffer, singleByte, readNextByte](
					const asio::error_code& errorCode, size_t /*bytesTransferred*/) {

					if (errorCode) {
						if (errorCode != asio::error::eof && errorCode != asio::error::operation_aborted) {
							AX_CORE_TRACE("Read error from {}: {}", RemoteAddress(), errorCode.message());
						}
						Disconnect();
						return;
					}

					// Decrypt byte with proper synchronization
					uint8_t decryptedByte = *singleByte;
					{
						std::lock_guard<std::mutex> lock(m_WriteMutex);
						if (m_Cipher) {
							auto decrypted = m_Cipher->Decrypt({*singleByte});
							if (decrypted.empty()) {
								Disconnect("Decryption failed");
								return;
							}
							decryptedByte = decrypted[0];
						}
					}

					lengthBuffer->push_back(decryptedByte);

					if ((decryptedByte & 0x80) == 0) {
						NetworkBuffer buffer(*lengthBuffer);
						int32_t frameLength = buffer.ReadVarInt();

						if (frameLength <= 0 || frameLength > 0x200000) {
							Disconnect("Invalid frame length");
							return;
						}

						ReadFrameBody(frameLength);
					} else if (lengthBuffer->size() >= 5) {
						Disconnect("VarInt too long");
					} else {
						(*readNextByte)();
					}
				});
		};

		(*readNextByte)();
	}

	void Connection::ReadFrameBody(int32_t frameLength) {
		auto self = shared_from_this();
		auto bodyBuffer = std::make_shared<std::vector<uint8_t>>(frameLength);

		asio::async_read(m_Socket, asio::buffer(*bodyBuffer),
			[this, self, bodyBuffer](const asio::error_code& errorCode, size_t /*bytesTransferred*/) {
				if (errorCode) {
					if (errorCode != asio::error::eof && errorCode != asio::error::operation_aborted) {
						AX_CORE_TRACE("Read error from {}: {}", RemoteAddress(), errorCode.message());
					}
					Disconnect();
					return;
				}

				std::vector<uint8_t> data = std::move(*bodyBuffer);

				// Decrypt with proper synchronization
				{
					std::lock_guard<std::mutex> lock(m_WriteMutex);
					if (m_Cipher) {
						data = m_Cipher->Decrypt(data);
					}
				}

				ProcessPacket(std::move(data));

				if (m_Connected.load(std::memory_order_acquire)) {
					ReadFrameLength();
				}
			});
	}

	void Connection::ProcessPacket(std::vector<uint8_t> data) {
		int32_t compressionThreshold = m_CompressionThreshold.load(std::memory_order_acquire);

		if (compressionThreshold >= 0) {
			// Compressed format: DataLength (VarInt) + CompressedData
			NetworkBuffer compressedBuffer(std::move(data));
			int32_t dataLength = compressedBuffer.ReadVarInt();

			if (dataLength == 0) {
				// Below threshold — rest is uncompressed packet data
				auto remaining = compressedBuffer.ReadRemainingBytes();
				NetworkBuffer buffer(std::move(remaining));
				int32_t packetId = buffer.ReadVarInt();
				InvokePacketHandler(packetId, buffer);
			} else {
				// Decompress
				auto compressedData = compressedBuffer.ReadRemainingBytes();
				std::vector<uint8_t> decompressed(dataLength);
				uLongf decompressedSize = static_cast<uLongf>(dataLength);

				int result = uncompress(decompressed.data(), &decompressedSize,
					compressedData.data(), static_cast<uLong>(compressedData.size()));

				if (result != Z_OK) {
					Disconnect("Decompression failed");
					return;
				}

				decompressed.resize(decompressedSize);
				NetworkBuffer buffer(std::move(decompressed));
				int32_t packetId = buffer.ReadVarInt();
				InvokePacketHandler(packetId, buffer);
			}
		} else {
			NetworkBuffer buffer(std::move(data));
			int32_t packetId = buffer.ReadVarInt();
			InvokePacketHandler(packetId, buffer);
		}
	}

}
