#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Configuration::Clientbound {

class FinishConfigurationPacket : public Packet<FinishConfigurationPacket,
	PID_CONFIGURATION_CB_FINISHCONFIGURATION> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer&) override {}

    AX_START_FIELDS()
    
    AX_END_FIELDS()
};

} // namespace Axiom::Configuration::Clientbound
