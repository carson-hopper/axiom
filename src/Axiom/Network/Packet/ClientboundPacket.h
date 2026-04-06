#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>

namespace Axiom {

	class ClientboundPacket {
	public:
		virtual ~ClientboundPacket() = default;

		virtual int32_t PacketId() const = 0;
		virtual void Encode(NetworkBuffer& buffer) const = 0;
	};

}
