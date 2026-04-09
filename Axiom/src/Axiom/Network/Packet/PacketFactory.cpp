#include "axpch.h"
#include "Axiom/Network/Packet/PacketFactory.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

    std::unordered_map<
        PacketState,
        std::unordered_map<int32_t, PacketHandlerFn>> PacketFactory::s_Handlers;

    void PacketFactory::DispatchPacket(PacketState state,
                                       int32_t packetId,
                                       Ref<Connection>& connection,
                                       PacketContext& context,
                                       NetworkBuffer& buffer) {
        auto stateIterator = s_Handlers.find(state);
        if (stateIterator == s_Handlers.end()) {
            AX_CORE_WARN("No handlers registered for state {0}",
                         static_cast<int32_t>(state));
            return;
        }

        auto packetIterator = stateIterator->second.find(packetId);
        if (packetIterator == stateIterator->second.end()) {
            AX_CORE_WARN("Unhandled packet id {0} in state {1}",
                         packetId, static_cast<int32_t>(state));
            return;
        }

        packetIterator->second(connection, context, buffer);
    }

}
