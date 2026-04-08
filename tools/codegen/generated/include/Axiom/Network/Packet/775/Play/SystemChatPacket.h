#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Chat/ChatComponent.h"

namespace Axiom {

class SystemChatPacket : public ClientboundPacket {
public:
    static constexpr int32_t PacketId = 103;
    static constexpr ConnectionState PacketState = ConnectionState::Play;

    SystemChatPacket() = default;
    SystemChatPacket(SystemChatPacket&&) = default;
    SystemChatPacket& operator=(SystemChatPacket&&) = default;

    SystemChatPacket(Ref<ChatComponent> message, bool overlay)
        : mMessage(std::move(message)), mOverlay(std::move(overlay))
    {}

    Ref<ChatComponent> GetMessage() const { return mMessage; }
    void SetMessage(Ref<ChatComponent> value) { mMessage = std::move(value); }
    bool IsOverlay() const { return mOverlay; }
    void SetOverlay(bool value) { mOverlay = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    Ref<ChatComponent> mMessage;
    bool mOverlay;
};

} // namespace Axiom