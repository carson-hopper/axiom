#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/PacketDirection.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace Axiom {

    class Connection;
    class PacketContext;
    class NetworkBuffer;

    class IChainablePacket {
    public:
        virtual ~IChainablePacket() = default;

        virtual int32_t GetPacketId() const = 0;
        virtual PacketDirection GetDirection() const = 0;

        virtual void Write(NetworkBuffer& buffer) = 0;

        virtual std::optional<std::vector<Ref<IChainablePacket>>>
            HandleImpl(const Ref<Connection>& connection,
                       PacketContext& context,
                       NetworkBuffer& buffer) = 0;
    };

}
