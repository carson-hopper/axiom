#include "PlayerChatPacket.h"

namespace Axiom {

void PlayerChatPacket<775>::Encode(NetworkBuffer& buffer) const {
	// UUID: write as two int64_t (high bits, low bits)
	// TODO: expand UUID to 128-bit when the type is updated
	buffer.WriteLong(static_cast<int64_t>(static_cast<uint64_t>(m_SenderUuid)));
	buffer.WriteLong(0);

	if (m_Signature.has_value()) {
		buffer.WriteBoolean(true);
		buffer.WriteVarInt(static_cast<int32_t>(m_Signature.value().size()));
		buffer.WriteBytes(m_Signature.value());
	} else {
		buffer.WriteBoolean(false);
	}

	buffer.WriteString(m_Message->ToJson());
	buffer.WriteLong(m_Timestamp);
	buffer.WriteLong(m_Salt);

	buffer.WriteVarInt(static_cast<int32_t>(m_PreviousMessages.size()));
	for (int32_t msg : m_PreviousMessages) {
		buffer.WriteVarInt(msg);
	}

	if (m_UnsignedContent.has_value()) {
		buffer.WriteBoolean(true);
		buffer.WriteString(m_UnsignedContent.value()->ToJson());
	} else {
		buffer.WriteBoolean(false);
	}

	buffer.WriteVarInt(m_FilterType);
	buffer.WriteVarInt(m_ChatType);
	buffer.WriteString(m_SenderName);

	if (m_TargetName.has_value()) {
		buffer.WriteBoolean(true);
		buffer.WriteString(m_TargetName.value());
	} else {
		buffer.WriteBoolean(false);
	}
}

}
