#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Configuration::Clientbound {

class UpdateTagsPacket : public Packet<UpdateTagsPacket,
	PID_CONFIGURATION_CB_UPDATETAGS> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	AX_START_FIELDS()
		AX_DECLARE(TagCount)
	AX_END_FIELDS()

	AX_FIELD_WITH_DEFAULT(TagCount, Net::VarInt, 0)
};

} // namespace Axiom::Configuration::Clientbound
