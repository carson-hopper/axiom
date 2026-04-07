#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class ChunkBatchReceivedPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::ChunkBatchReceived;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			chunksPerTick = buffer.ReadFloat();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		float chunksPerTick = 0.0f;
	};

}
