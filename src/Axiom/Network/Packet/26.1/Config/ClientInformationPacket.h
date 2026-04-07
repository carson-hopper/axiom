#pragma once

/**
 * @file ClientInformationPacket.h
 * @brief Client settings packet.
 *
 * Contains player preferences like locale, view distance, chat settings, etc.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

#include <string>

namespace Axiom {

	PACKET_DECL_BEGIN(ClientInformationPacket, Configuration, Serverbound::Config::ClientInformation)
		PACKET_FIELD_STRING(Locale)
		PACKET_FIELD_INT8(ViewDistance)
		PACKET_FIELD_INT32(ChatMode)
		PACKET_FIELD_BOOL(ChatColors)
		PACKET_FIELD_INT8(DisplayedSkinParts)
		PACKET_FIELD_INT32(MainHand)
		PACKET_FIELD_BOOL(EnableTextFiltering)
		PACKET_FIELD_BOOL(AllowServerListings)
		PACKET_FIELD_INT32(ParticleStatus)
	PACKET_DECL_END()

}