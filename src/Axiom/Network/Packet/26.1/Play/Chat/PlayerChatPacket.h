#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/UUID.h"
#include "Axiom/Network/Packet/PacketVersioned.h"

#include <optional>
#include <vector>

namespace Axiom {

PACKET_VERSIONED_CB(PlayerChatPacket, 775, Play, 0x41)
	FIELD(UUID, SenderUuid, {})
	FIELD_OPTIONAL(std::vector<uint8_t>, Signature)
	FIELD(Ref<ChatComponent>, Message, nullptr)
	FIELD(int64_t, Timestamp, 0)
	FIELD(int64_t, Salt, 0)
	FIELD_ARRAY(int32_t, PreviousMessages)
	FIELD_OPTIONAL(Ref<ChatComponent>, UnsignedContent)
	FIELD(int32_t, FilterType, 0)
	FIELD(int32_t, ChatType, 0)
	FIELD_STRING(SenderName)
	FIELD_OPTIONAL(std::string, TargetName)
PACKET_VERSIONED_END(PlayerChatPacket, 775)

}
