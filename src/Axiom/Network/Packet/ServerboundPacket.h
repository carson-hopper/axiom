#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>

namespace Axiom {

	class Connection;
	class PacketContext;

	class ServerboundPacket {
	public:
		virtual ~ServerboundPacket() = default;

		virtual int32_t GetPacketId() const { return -1; }
		virtual void Encode(NetworkBuffer& /*buffer*/) const {}
		virtual void Decode(NetworkBuffer& buffer) = 0;
		virtual void Handle(Ref<Connection> connection, PacketContext& context) = 0;
	};

}
