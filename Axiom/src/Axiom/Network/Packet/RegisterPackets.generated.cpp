#include "axpch.h"
#include "RegisterPackets.generated.h"

#include "Axiom/Network/Packet/PacketFactory.h"

namespace Axiom {

    void PacketFactory::RegisterAll() {
        RegisterDefaultParser<Axiom::Configuration::Serverbound::SelectKnownPacksPacket>();
        RegisterDefaultParser<Axiom::Configuration::Serverbound::KeepAlivePacket>();
        RegisterDefaultParser<Axiom::Configuration::Serverbound::ClientInformationPacket>();
        RegisterDefaultParser<Axiom::Configuration::Serverbound::FinishConfigurationPacket>();
        RegisterDefaultParser<Axiom::Handshake::Serverbound::HandshakePacket>();
        RegisterDefaultParser<Axiom::Status::Serverbound::PingRequestPacket>();
        RegisterDefaultParser<Axiom::Status::Serverbound::StatusRequestPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::UseItemOnPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::ChatPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::MovePlayerPositionRotationPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::SetCreativeModeSlotPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::SetCarriedItemPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::PlayerActionPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::ChunkBatchReceivedPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::MovePlayerPositionPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::AcceptTeleportationPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::ClientTickEndPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::MovePlayerStatusOnlyPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::KeepAlivePacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::PlayerLoadedPacket>();
        RegisterDefaultParser<Axiom::Play::Serverbound::MovePlayerRotationPacket>();
        RegisterDefaultParser<Axiom::Login::Serverbound::LoginAcknowledgedPacket>();
        RegisterDefaultParser<Axiom::Login::Serverbound::LoginHelloPacket>();
        RegisterDefaultParser<Axiom::Login::Serverbound::EncryptionResponsePacket>();
    }

}
