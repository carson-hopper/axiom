#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Network/Packet/PacketVersioned.h"

namespace Axiom {

PACKET_VERSIONED_CB(SystemChatPacket, 775, Play, 0x67)
	FIELD(Ref<ChatComponent>, Message, nullptr)
	FIELD_BOOL(ActionBar)
PACKET_VERSIONED_END(SystemChatPacket, 775)

}
