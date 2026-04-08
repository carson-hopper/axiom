#include "PlayerChatPacket.h"
namespace Axiom {

void PlayerChatPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteUUID(mSenderUuid);
    buffer.WriteOptional(&NetworkBuffer::WriteByteArray)(mSignature);
    buffer.WriteChatComponent(mMessage);
    buffer.WriteLong(mTimestamp);
    buffer.WriteLong(mSalt);
    buffer.WriteVarIntArray(mPreviousMessages);
    buffer.WriteOptional(&NetworkBuffer::WriteChatComponent)(mUnsignedContent);
    buffer.WriteVarInt(mFilterType);
    buffer.WriteVarInt(mChatType);
    buffer.WriteString(mSenderName);
    buffer.WriteOptional(&NetworkBuffer::WriteString)(mTargetName);
}

template class PlayerChatPacket<775>;

} // namespace Axiom