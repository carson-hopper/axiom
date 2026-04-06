#include "Connection.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	Connection::Connection(asio::ip::tcp::socket socket)
		: m_Socket(std::move(socket)) {}

	Connection::~Connection() {
		Disconnect();
	}

	void Connection::Start() {
		m_Connected = true;
		AX_CORE_TRACE("Connection opened from {}", RemoteAddress());
		ReadLoop();
	}

	void Connection::Disconnect(const std::string& reason) {
		if (!m_Connected) {
			return;
		}
		m_Connected = false;

		if (!reason.empty()) {
			AX_CORE_INFO("Disconnecting {}: {}", RemoteAddress(), reason);
		}

		asio::error_code errorCode;
		m_Socket.shutdown(asio::ip::tcp::socket::shutdown_both, errorCode);
		m_Socket.close(errorCode);
	}

	void Connection::SendRawPacket(int32_t packetId, const NetworkBuffer& payload) {
		if (!m_Connected) {
			return;
		}

		NetworkBuffer packet;
		packet.WriteVarInt(packetId);
		packet.WriteBytes(payload.Data());

		NetworkBuffer frame;
		frame.WriteVarInt(static_cast<int32_t>(packet.Size()));
		frame.WriteBytes(packet.Data());

		std::lock_guard<std::mutex> lock(m_WriteMutex);
		asio::error_code errorCode;
		asio::write(m_Socket, asio::buffer(frame.Data()), errorCode);

		if (errorCode) {
			AX_CORE_TRACE("Write error to {}: {}", RemoteAddress(), errorCode.message());
			Disconnect();
		}
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
		auto self = shared_from_this();
		auto lengthBuffer = std::make_shared<std::vector<uint8_t>>();

		// Read VarInt one byte at a time
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

					lengthBuffer->push_back(*singleByte);

					if ((*singleByte & 0x80) == 0) {
						// VarInt complete — decode it
						NetworkBuffer buffer(*lengthBuffer);
						int32_t frameLength = buffer.ReadVarInt();

						if (frameLength <= 0 || frameLength > 2097152) {
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

				ProcessPacket(std::move(*bodyBuffer));

				if (m_Connected) {
					ReadFrameLength();
				}
			});
	}

	void Connection::ProcessPacket(std::vector<uint8_t> data) {
		NetworkBuffer buffer(std::move(data));
		int32_t packetId = buffer.ReadVarInt();

		if (m_PacketHandler) {
			m_PacketHandler(shared_from_this(), packetId, buffer);
		}
	}

}
