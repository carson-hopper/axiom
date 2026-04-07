#include "SetCarriedItemPacket.h"

#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

PACKET_DECODE_BEGIN(SetCarriedItemPacket)
    READ_SHORT(m_Slot)
PACKET_DECODE_END()

PACKET_HANDLE_BEGIN(SetCarriedItemPacket)
    auto player = context.Players().GetPlayer(connection->Id());
    if (player) {
        player->SetSelectedSlot(m_Slot);
    }
PACKET_HANDLE_END()

PACKET_INSTANTIATE(SetCarriedItemPacket, 775)

}
