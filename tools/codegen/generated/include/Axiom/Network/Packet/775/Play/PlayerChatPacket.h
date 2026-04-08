#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/Packet/ClientboundPacket.h"
#include "Axiom/Network/NetworkBuffer.h"
#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/UUID.h"
#include <optional>
#include <vector>

namespace Axiom {

class PlayerChatPacket : public ClientboundPacket {
public:
    static constexpr int32_t PacketId = 65;
    static constexpr ConnectionState PacketState = ConnectionState::Play;

    PlayerChatPacket() = default;
    PlayerChatPacket(PlayerChatPacket&&) = default;
    PlayerChatPacket& operator=(PlayerChatPacket&&) = default;

    PlayerChatPacket(UUID sender_uuid, std::optional<std::vector<uint8_t>> signature, Ref<ChatComponent> message, int64_t timestamp, int64_t salt, std::vector<int32_t> previous_messages, std::optional<Ref<ChatComponent>> unsigned_content, int32_t filter_type, int32_t chat_type, std::string sender_name, std::optional<std::string> target_name)
        : m_sender_uuid(std::move(sender_uuid)), m_signature(std::move(signature)), m_message(std::move(message)), m_timestamp(std::move(timestamp)), m_salt(std::move(salt)), m_previous_messages(std::move(previous_messages)), m_unsigned_content(std::move(unsigned_content)), m_filter_type(std::move(filter_type)), m_chat_type(std::move(chat_type)), m_sender_name(std::move(sender_name)), m_target_name(std::move(target_name))
    {}

    UUID GetSenderUuid() const { return m_sender_uuid; }
    void SetSenderUuid(UUID value) { m_sender_uuid = std::move(value); }
    const std::optional<std::vector<uint8_t>>& GetSignature() const { return m_signature; }
    void SetSignature(std::optional<std::vector<uint8_t>> value) { m_signature = std::move(value); }
    Ref<ChatComponent> GetMessage() const { return m_message; }
    void SetMessage(Ref<ChatComponent> value) { m_message = std::move(value); }
    int64_t GetTimestamp() const { return m_timestamp; }
    void SetTimestamp(int64_t value) { m_timestamp = std::move(value); }
    int64_t GetSalt() const { return m_salt; }
    void SetSalt(int64_t value) { m_salt = std::move(value); }
    std::vector<int32_t> GetPreviousMessages() const { return m_previous_messages; }
    void SetPreviousMessages(std::vector<int32_t> value) { m_previous_messages = std::move(value); }
    const std::optional<Ref<ChatComponent>>& GetUnsignedContent() const { return m_unsigned_content; }
    void SetUnsignedContent(std::optional<Ref<ChatComponent>> value) { m_unsigned_content = std::move(value); }
    int32_t GetFilterType() const { return m_filter_type; }
    void SetFilterType(int32_t value) { m_filter_type = std::move(value); }
    int32_t GetChatType() const { return m_chat_type; }
    void SetChatType(int32_t value) { m_chat_type = std::move(value); }
    std::string GetSenderName() const { return m_sender_name; }
    void SetSenderName(std::string value) { m_sender_name = std::move(value); }
    const std::optional<std::string>& GetTargetName() const { return m_target_name; }
    void SetTargetName(std::optional<std::string> value) { m_target_name = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    UUID m_sender_uuid;
    std::optional<std::vector<uint8_t>> m_signature;
    Ref<ChatComponent> m_message;
    int64_t m_timestamp;
    int64_t m_salt;
    std::vector<int32_t> m_previous_messages;
    std::optional<Ref<ChatComponent>> m_unsigned_content;
    int32_t m_filter_type;
    int32_t m_chat_type;
    std::string m_sender_name;
    std::optional<std::string> m_target_name;
};

} // namespace Axiom