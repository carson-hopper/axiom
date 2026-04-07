#pragma once

/**
 * @file FinishConfigurationPacket.h
 *
 * Client acknowledges configuration is complete and is ready to enter play.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

DEFINE_SIMPLE_PACKET(FinishConfigurationPacket, Configuration, Serverbound::Config::FinishConfiguration)

}
