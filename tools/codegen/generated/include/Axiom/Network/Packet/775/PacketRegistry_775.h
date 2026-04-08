#pragma once

#include "Axiom/Network/Packet/PacketRegistry.h"
namespace Axiom {

// Protocol version 775
inline void RegisterPackets_775(PacketRegistry& registry) {
    registry.Register<775, HandshakePacket<775>>();
    registry.Register<775, StatusRequestPacket<775>>();
    registry.Register<775, StatusResponsePacket<775>>();
    registry.Register<775, StatusPingPacket<775>>();
    registry.Register<775, StatusPongPacket<775>>();
    registry.Register<775, LoginStartPacket<775>>();
    registry.Register<775, LoginSuccessPacket<775>>();
    registry.Register<775, KeepAlivePacket<775>>();
    registry.Register<775, KeepAliveResponsePacket<775>>();
    registry.Register<775, PlayerChatPacket<775>>();
    registry.Register<775, SystemChatPacket<775>>();
    registry.Register<775, SetPlayerPositionPacket<775>>();
    registry.Register<775, ChunkDataPacket<775>>();
}

} // namespace Axiom