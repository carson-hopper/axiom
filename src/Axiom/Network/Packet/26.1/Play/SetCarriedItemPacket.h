#pragma once

/**
 * @file SetCarriedItemPacket.h
 *
 * Client tells the server which hotbar slot is selected (0-8).
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

PACKET_DECL_BEGIN(SetCarriedItemPacket, Play, Serverbound::Play::SetCarriedItem)
    PACKET_FIELD_INT32(Slot)
PACKET_DECL_END()

}
