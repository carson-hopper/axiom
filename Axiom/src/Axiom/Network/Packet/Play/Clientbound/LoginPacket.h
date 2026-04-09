#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Play::Clientbound {

/**
 * The Login (Join Game) packet sent when a player
 * enters the Play state. Contains world metadata,
 * dimension info, and initial game settings.
 */
class LoginPacket : public Packet<LoginPacket, PID_PLAY_CB_LOGIN> {
public:
    LoginPacket() = default;

    LoginPacket(int32_t entityId, int32_t maxPlayers,
                int32_t viewDistance, int32_t simulationDistance,
                uint8_t gameMode, int32_t seaLevel) {
        m_EntityId = entityId;
        m_MaxPlayers.Value = maxPlayers;
        m_ViewDistance.Value = viewDistance;
        m_SimulationDistance.Value = simulationDistance;
        m_GameMode = gameMode;
        m_SeaLevel.Value = seaLevel;
    }

    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

    /**
     * Manual write because EntityId uses Int
     * (not VarInt), dimension names are a
     * length-prefixed string array, and
     * several fields use raw byte writes.
     */
    void Write(NetworkBuffer& buffer) override {
        buffer.WriteInt(m_EntityId);
        buffer.WriteBoolean(m_IsHardcore);
        buffer.WriteVarInt(m_DimensionCount);
        for (int32_t index = 0; index < m_DimensionCount; ++index) {
            buffer.WriteString(m_DimensionNames[index]);
        }
        buffer.WriteVarInt(m_MaxPlayers.Value);
        buffer.WriteVarInt(m_ViewDistance.Value);
        buffer.WriteVarInt(m_SimulationDistance.Value);
        buffer.WriteBoolean(m_ReducedDebugInfo);
        buffer.WriteBoolean(m_EnableRespawnScreen);
        buffer.WriteBoolean(m_DoLimitedCrafting);
        buffer.WriteVarInt(m_DimensionType);
        buffer.WriteString(m_DimensionName);
        buffer.WriteLong(m_HashedSeed);
        buffer.WriteByte(m_GameMode);
        buffer.WriteByte(m_PreviousGameMode);
        buffer.WriteBoolean(m_IsDebug);
        buffer.WriteBoolean(m_IsFlat);
        buffer.WriteBoolean(m_HasDeathLocation);
        buffer.WriteVarInt(m_PortalCooldown);
        buffer.WriteVarInt(m_SeaLevel.Value);
        buffer.WriteBoolean(m_EnforcesSecureChat);
    }

    auto Fields() { return std::tuple<>(); }

private:
    int32_t m_EntityId = 0;
    bool m_IsHardcore = false;
    int32_t m_DimensionCount = 1;
    std::vector<std::string> m_DimensionNames = { "minecraft:overworld" };
    Field<Net::VarInt> m_MaxPlayers;
    Field<Net::VarInt> m_ViewDistance;
    Field<Net::VarInt> m_SimulationDistance;
    bool m_ReducedDebugInfo = false;
    bool m_EnableRespawnScreen = true;
    bool m_DoLimitedCrafting = false;
    int32_t m_DimensionType = 1;
    std::string m_DimensionName = "minecraft:overworld";
    int64_t m_HashedSeed = 0;
    uint8_t m_GameMode = 1;
    uint8_t m_PreviousGameMode = 0xFF;
    bool m_IsDebug = false;
    bool m_IsFlat = false;
    bool m_HasDeathLocation = false;
    int32_t m_PortalCooldown = 0;
    Field<Net::VarInt> m_SeaLevel;
    bool m_EnforcesSecureChat = false;
};

} // namespace Axiom::Play::Clientbound
