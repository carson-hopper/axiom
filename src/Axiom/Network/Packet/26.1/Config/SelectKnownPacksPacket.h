#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

#include <string>
#include <vector>

namespace Axiom {

	struct KnownPack {
		std::string namespaceName;
		std::string identifier;
		std::string version;
	};

	template<int32_t Version = PROTOCOL_VERSION>
	class SelectKnownPacksPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Config::SelectKnownPacks;
		static constexpr ConnectionState PacketState = ConnectionState::Configuration;

		void Decode(NetworkBuffer& buffer) override {
			int32_t count = buffer.ReadVarInt();
			knownPacks.resize(count);
			for (auto& pack : knownPacks) {
				pack.namespaceName = buffer.ReadString(256);
				pack.identifier = buffer.ReadString(256);
				pack.version = buffer.ReadString(256);
			}
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		std::vector<KnownPack> knownPacks;
	};

}
