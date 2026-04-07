#pragma once

/**
 * @file SelectKnownPacksPacket.h
 *
 * Client selects which data packs it already knows to avoid resending.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

#include <string>
#include <vector>

namespace Axiom {

struct KnownPack {
	std::string namespaceName;
	std::string identifier;
	std::string version;
};

PACKET_DECL_BEGIN(SelectKnownPacksPacket, Configuration, Serverbound::Config::SelectKnownPacks)
	PACKET_FIELD_UINT8(Flags)
	PACKET_FIELD(std::vector<KnownPack>, KnownPacks, {})
PACKET_DECL_END()

}