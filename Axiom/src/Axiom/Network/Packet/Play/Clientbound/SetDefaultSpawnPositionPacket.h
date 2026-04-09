#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Play::Clientbound {

/**
 * Sets the world spawn point used by the
 * compass and initial respawn location.
 * Writes dimension name, block position,
 * then yaw/pitch as a float pair.
 */
class SetDefaultSpawnPositionPacket : public Packet<SetDefaultSpawnPositionPacket,
    PID_PLAY_CB_SETDEFAULTSPAWNPOSITION> {
public:
    SetDefaultSpawnPositionPacket() = default;

    SetDefaultSpawnPositionPacket(const std::string& dimensionName,
                                 int32_t blockX, int32_t blockY, int32_t blockZ,
                                 float yaw, float pitch) {
        m_DimensionName = dimensionName;
        m_BlockX = blockX;
        m_BlockY = blockY;
        m_BlockZ = blockZ;
        m_Yaw = yaw;
        m_Pitch = pitch;
    }

    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

    /**
     * Manual write because the wire format
     * is String + BlockPosition + Vector2,
     * which has no single Net:: composite.
     */
    void Write(NetworkBuffer& buffer) override {
        buffer.WriteString(m_DimensionName);
        buffer.WriteBlockPosition(m_BlockX, m_BlockY, m_BlockZ);
        buffer.WriteVector2(m_Yaw, m_Pitch);
    }

    auto Fields() { return std::tuple<>(); }

private:
    std::string m_DimensionName = "minecraft:overworld";
    int32_t m_BlockX = 0;
    int32_t m_BlockY = 0;
    int32_t m_BlockZ = 0;
    float m_Yaw = 0.0f;
    float m_Pitch = 0.0f;
};

} // namespace Axiom::Play::Clientbound
