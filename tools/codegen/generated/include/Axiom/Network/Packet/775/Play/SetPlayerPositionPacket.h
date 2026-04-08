#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

class SetPlayerPositionPacket : public ServerboundPacket {
public:
    static constexpr int32_t PacketId = 26;
    static constexpr ConnectionState PacketState = ConnectionState::Play;

    SetPlayerPositionPacket() = default;
    SetPlayerPositionPacket(SetPlayerPositionPacket&&) = default;
    SetPlayerPositionPacket& operator=(SetPlayerPositionPacket&&) = default;

    SetPlayerPositionPacket(double x, double y, double z, bool on_ground)
        : m_x(std::move(x)), m_y(std::move(y)), m_z(std::move(z)), m_on_ground(std::move(on_ground))
    {}

    double GetX() const { return m_x; }
    void SetX(double value) { m_x = std::move(value); }
    double GetY() const { return m_y; }
    void SetY(double value) { m_y = std::move(value); }
    double GetZ() const { return m_z; }
    void SetZ(double value) { m_z = std::move(value); }
    bool IsOnGround() const { return m_on_ground; }
    void SetOnGround(bool value) { m_on_ground = std::move(value); }

    void Deserialize(NetworkBuffer& buffer) override;
    void Handle(Ref<Connection> connection, PacketContext& context) override;

private:
    double m_x;
    double m_y;
    double m_z;
    bool m_on_ground;
};

} // namespace Axiom