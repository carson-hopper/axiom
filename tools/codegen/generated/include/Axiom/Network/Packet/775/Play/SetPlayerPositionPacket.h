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

    SetPlayerPositionPacket(double x, double y, double z, bool onGround)
        : mX(std::move(x)), mY(std::move(y)), mZ(std::move(z)), mOnGround(std::move(onGround))
    {}

    double GetX() const { return mX; }
    void SetX(double value) { mX = std::move(value); }
    double GetY() const { return mY; }
    void SetY(double value) { mY = std::move(value); }
    double GetZ() const { return mZ; }
    void SetZ(double value) { mZ = std::move(value); }
    bool IsOnGround() const { return mOnGround; }
    void SetOnGround(bool value) { mOnGround = std::move(value); }

    void Deserialize(NetworkBuffer& buffer) override;
    void Handle(Ref<Connection> connection, PacketContext& context) override;

private:
    double mX;
    double mY;
    double mZ;
    bool mOnGround;
};

} // namespace Axiom