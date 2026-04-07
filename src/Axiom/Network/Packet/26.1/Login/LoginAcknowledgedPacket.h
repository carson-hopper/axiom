#pragma once

/**
 * @file LoginAcknowledgedPacket.h
 *
 * Client acknowledges successful login and transitions to configuration.
 */

#include "Axiom/Network/Packet/PacketMacros.h"

namespace Axiom {

DEFINE_SIMPLE_PACKET(LoginAcknowledgedPacket, Login, Serverbound::Login::LoginAcknowledged)

}
