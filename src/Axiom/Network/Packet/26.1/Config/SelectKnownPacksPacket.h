#pragma once

/**
 * @file SelectKnownPacksPacket.h
 *
 * Client selects which data packs it already knows to avoid resending.
 */

#include "Axiom/Network/Packet/PacketVersioned.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Axiom {

struct KnownPack {
	std::string m_namespaceName;
	std::string m_identifier;
	std::string m_version;
};

PACKET_VERSIONED(SelectKnownPacksPacket, 775, Serverbound, Configuration, 0)
	FIELD(uint8_t, Flags, 0)
	FIELD_ARRAY(KnownPack, KnownPacks)
PACKET_VERSIONED_END(SelectKnownPacksPacket, 775)

}
