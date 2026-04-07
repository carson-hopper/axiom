#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>
#include <vector>

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class EncryptionResponsePacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Login::Key;
		static constexpr ConnectionState PacketState = ConnectionState::Login;

		void Decode(NetworkBuffer& buffer) override {
			int32_t secretLength = buffer.ReadVarInt();
			encryptedSharedSecret = buffer.ReadBytes(secretLength);

			int32_t tokenLength = buffer.ReadVarInt();
			encryptedVerifyToken = buffer.ReadBytes(tokenLength);
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		std::vector<uint8_t> encryptedSharedSecret;
		std::vector<uint8_t> encryptedVerifyToken;
	};

}
