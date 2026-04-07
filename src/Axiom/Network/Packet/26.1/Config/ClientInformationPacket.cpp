#include "ClientInformationPacket.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom {

	// ----- Packet Decoding ------------------------------------------

	PACKET_DECODE_BEGIN(ClientInformationPacket)
		READ_STRING_MAX(m_Locale, 16);
		READ_BYTE(m_ViewDistance);
		READ_VARINT(m_ChatMode);
		READ_BOOL(m_ChatColors);
		READ_BYTE(m_DisplayedSkinParts);
		READ_VARINT(m_MainHand);
		READ_BOOL(m_EnableTextFiltering);
		READ_BOOL(m_AllowServerListings);
		READ_VARINT(m_ParticleStatus);
	PACKET_DECODE_END()

	// ----- Packet Handling ------------------------------------------

	PACKET_HANDLE_BEGIN(ClientInformationPacket)
		AX_CORE_TRACE("Client info from {}: locale={}, viewDistance={}, mainHand={}",
			connection->RemoteAddress(), m_Locale, m_ViewDistance, m_MainHand);
	PACKET_HANDLE_END()

	// ----- Template Instantiation -----------------------------------

	PACKET_INSTANTIATE(ClientInformationPacket, 775)

}