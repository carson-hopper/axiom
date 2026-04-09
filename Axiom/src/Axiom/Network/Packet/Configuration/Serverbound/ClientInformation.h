#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

namespace Axiom::Configuration::Serverbound {

class ClientInformationPacket : public Packet<ClientInformationPacket,
    PID_CONFIGURATION_SB_CLIENTINFORMATION> {
public:
    std::optional<std::vector<Ref<IChainablePacket>>>
    Handle(const Ref<Connection> &connection, PacketContext& /*context*/, NetworkBuffer& /*buffer*/) {
        AX_CORE_TRACE(
            "Client info from {}: locale={}, viewDistance={}, mainHand={}",
            connection->RemoteAddress(), m_Locale.Value,
            m_ViewDistance.Value, m_MainHand.Value);

        return std::nullopt;
    }

    AX_START_FIELDS()
        AX_DECLARE(Locale),
        AX_DECLARE(ViewDistance),
        AX_DECLARE(ChatMode),
        AX_DECLARE(ChatColors),
        AX_DECLARE(DisplayedSkinParts),
        AX_DECLARE(MainHand),
        AX_DECLARE(EnableTextFiltering),
        AX_DECLARE(AllowServerListings),
        AX_DECLARE(ParticleStatus)
    AX_END_FIELDS()

    AX_FIELD(Locale, std::string);
    AX_FIELD(ViewDistance, int8_t);
    AX_FIELD(ChatMode, int32_t);
    AX_FIELD(ChatColors, bool);
    AX_FIELD(DisplayedSkinParts, uint8_t);
    AX_FIELD(MainHand, int32_t);
    AX_FIELD(EnableTextFiltering, bool);
    AX_FIELD(AllowServerListings, bool);
    AX_FIELD(ParticleStatus, int32_t);
};

} // namespace Axiom::Configuration::Serverbound
