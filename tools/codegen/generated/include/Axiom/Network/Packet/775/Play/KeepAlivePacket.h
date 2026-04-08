#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom {

class KeepAlivePacket : public ServerboundPacket {
public:
    static constexpr int32_t PacketId = 36;
    static constexpr ConnectionState PacketState = ConnectionState::Play;

    KeepAlivePacket() = default;
    KeepAlivePacket(KeepAlivePacket&&) = default;
    KeepAlivePacket& operator=(KeepAlivePacket&&) = default;

    KeepAlivePacket(int64_t id)
        : mId(std::move(id))
    {}

    int64_t GetId() const { return mId; }
    void SetId(int64_t value) { mId = std::move(value); }

    void Deserialize(NetworkBuffer& buffer) override;
    void Handle(Ref<Connection> connection, PacketContext& context) override;

private:
    int64_t mId;
};

} // namespace Axiom