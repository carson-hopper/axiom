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
        : m_message(std::move(message)), m_overlay(std::move(overlay))
    {}

    Ref<ChatComponent> GetMessage() const { return m_message; }
    void SetMessage(Ref<ChatComponent> value) { m_message = std::move(value); }
    bool IsOverlay() const { return m_overlay; }
    void SetOverlay(bool value) { m_overlay = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    Ref<ChatComponent> m_message;
    bool m_overlay;
};

} // namespace Axiom