#include "PlayerChatPacket.h"
namespace Axiom {

void PlayerChatPacket::Serialize(NetworkBuffer& buffer) const {
    buffer.WriteUUID(m_sender_uuid);
    buffer.WriteOptional(&NetworkBuffer::WriteByteArray)(m_signature);
    buffer.WriteChatComponent(m_message);
    buffer.WriteLong(m_timestamp);
    buffer.WriteLong(m_salt);
    buffer.WriteVarIntArray(m_previous_messages);
    buffer.WriteOptional(&NetworkBuffer::WriteChatComponent)(m_unsigned_content);
    buffer.WriteVarInt(m_filter_type);
    buffer.WriteVarInt(m_chat_type);
    buffer.WriteString(m_sender_name);
    buffer.WriteOptional(&NetworkBuffer::WriteString)(m_target_name);
}

template class PlayerChatPacket<775>;

} // namespace Axiom