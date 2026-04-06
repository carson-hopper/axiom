#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Network/Protocol.h"

#include <memory>

namespace Axiom {

	class Connection;
	class PacketContext;

	class ServerboundPacket {
	public:
		virtual ~ServerboundPacket() = default;

		virtual void Decode(NetworkBuffer& buffer) = 0;
		virtual void Handle(Ref<Connection> connection, PacketContext& context) = 0;
	};

}
