#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>

namespace Axiom {

	class Connection;
	class PacketContext;

	class ClientboundPacket {
	public:
		virtual ~ClientboundPacket() = default;

		virtual int32_t GetPacketId() const = 0;
		virtual void Encode(NetworkBuffer& buffer) const = 0;
		virtual void Decode(NetworkBuffer& /*buffer*/) {}
		virtual void Handle(Ref<Connection> /*connection*/, PacketContext& /*context*/) {}
	};

}
