#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <string>

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class ClientInformationPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Config::ClientInformation;
		static constexpr ConnectionState PacketState = ConnectionState::Configuration;

		void Decode(NetworkBuffer& buffer) override {
			locale = buffer.ReadString(16);
			viewDistance = buffer.ReadByte();
			chatMode = buffer.ReadVarInt();
			chatColors = buffer.ReadBoolean();
			displayedSkinParts = buffer.ReadByte();
			mainHand = buffer.ReadVarInt();
			enableTextFiltering = buffer.ReadBoolean();
			allowServerListings = buffer.ReadBoolean();
			particleStatus = buffer.ReadVarInt();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		std::string locale;
		int8_t viewDistance = 0;
		int32_t chatMode = 0;
		bool chatColors = true;
		uint8_t displayedSkinParts = 0;
		int32_t mainHand = 0;
		bool enableTextFiltering = false;
		bool allowServerListings = true;
		int32_t particleStatus = 0;
	};

}
