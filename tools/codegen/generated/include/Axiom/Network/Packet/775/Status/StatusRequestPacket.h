#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

class StatusRequestPacket : public ServerboundPacket {
public:
    static constexpr int32_t PacketId = 0;
    static constexpr ConnectionState PacketState = ConnectionState::Status;

    StatusRequestPacket() = default;
    StatusRequestPacket(StatusRequestPacket&&) = default;
    StatusRequestPacket& operator=(StatusRequestPacket&&) = default;



    void Deserialize(NetworkBuffer& buffer) override;
    void Handle(Ref<Connection> connection, PacketContext& context) override;

private:
};

} // namespace Axiom