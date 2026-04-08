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

    PlayerChatPacket(UUID senderUuid, std::optional<std::vector<uint8_t>> signature, Ref<ChatComponent> message, int64_t timestamp, int64_t salt, std::vector<int32_t> previousMessages, std::optional<Ref<ChatComponent>> unsignedContent, int32_t filterType, int32_t chatType, std::string senderName, std::optional<std::string> targetName)
        : mSenderUuid(std::move(senderUuid)), mSignature(std::move(signature)), mMessage(std::move(message)), mTimestamp(std::move(timestamp)), mSalt(std::move(salt)), mPreviousMessages(std::move(previousMessages)), mUnsignedContent(std::move(unsignedContent)), mFilterType(std::move(filterType)), mChatType(std::move(chatType)), mSenderName(std::move(senderName)), mTargetName(std::move(targetName))
    {}

    UUID GetSenderUuid() const { return mSenderUuid; }
    void SetSenderUuid(UUID value) { mSenderUuid = std::move(value); }
    const std::optional<std::vector<uint8_t>>& GetSignature() const { return mSignature; }
    void SetSignature(std::optional<std::vector<uint8_t>> value) { mSignature = std::move(value); }
    Ref<ChatComponent> GetMessage() const { return mMessage; }
    void SetMessage(Ref<ChatComponent> value) { mMessage = std::move(value); }
    int64_t GetTimestamp() const { return mTimestamp; }
    void SetTimestamp(int64_t value) { mTimestamp = std::move(value); }
    int64_t GetSalt() const { return mSalt; }
    void SetSalt(int64_t value) { mSalt = std::move(value); }
    std::vector<int32_t> GetPreviousMessages() const { return mPreviousMessages; }
    void SetPreviousMessages(std::vector<int32_t> value) { mPreviousMessages = std::move(value); }
    const std::optional<Ref<ChatComponent>>& GetUnsignedContent() const { return mUnsignedContent; }
    void SetUnsignedContent(std::optional<Ref<ChatComponent>> value) { mUnsignedContent = std::move(value); }
    int32_t GetFilterType() const { return mFilterType; }
    void SetFilterType(int32_t value) { mFilterType = std::move(value); }
    int32_t GetChatType() const { return mChatType; }
    void SetChatType(int32_t value) { mChatType = std::move(value); }
    std::string GetSenderName() const { return mSenderName; }
    void SetSenderName(std::string value) { mSenderName = std::move(value); }
    const std::optional<std::string>& GetTargetName() const { return mTargetName; }
    void SetTargetName(std::optional<std::string> value) { mTargetName = std::move(value); }

    int32_t GetPacketId() const override { return PacketId; }
    void Serialize(NetworkBuffer& buffer) const override;

private:
    UUID mSenderUuid;
    std::optional<std::vector<uint8_t>> mSignature;
    Ref<ChatComponent> mMessage;
    int64_t mTimestamp;
    int64_t mSalt;
    std::vector<int32_t> mPreviousMessages;
    std::optional<Ref<ChatComponent>> mUnsignedContent;
    int32_t mFilterType;
    int32_t mChatType;
    std::string mSenderName;
    std::optional<std::string> mTargetName;
};

} // namespace Axiom