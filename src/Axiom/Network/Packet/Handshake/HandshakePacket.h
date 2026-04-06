#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>
#include <string>

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class HandshakePacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = 0x00;
		static constexpr ConnectionState PacketState = ConnectionState::Handshake;

		void Decode(NetworkBuffer& buffer) override {
			protocolVersion = buffer.ReadVarInt();
			serverAddress = buffer.ReadString(255);
			serverPort = buffer.ReadUnsignedShort();
			nextState = buffer.ReadVarInt();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		int32_t protocolVersion = 0;
		std::string serverAddress;
		uint16_t serverPort = 0;
		int32_t nextState = 0;
	};

}
