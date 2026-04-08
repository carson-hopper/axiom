#pragma once

/**
 * @file ClientInformationPacket.h
 * @brief Client settings packet.
 *
 * Contains player preferences like locale, view distance, chat settings, etc.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

#include <string>

namespace Axiom {

PACKET_VERSIONED(ClientInformationPacket, 775, Serverbound, Configuration, 0x00)
	FIELD_STRING(Locale)
	FIELD(int8_t, ViewDistance, 0)
	FIELD(int32_t, ChatMode, 0)
	FIELD_BOOL(ChatColors)
	FIELD(uint8_t, DisplayedSkinParts, 0)
	FIELD(int32_t, MainHand, 0)
	FIELD_BOOL(EnableTextFiltering)
	FIELD_BOOL(AllowServerListings)
	FIELD(int32_t, ParticleStatus, 0)
PACKET_VERSIONED_END(ClientInformationPacket, 775)

}
