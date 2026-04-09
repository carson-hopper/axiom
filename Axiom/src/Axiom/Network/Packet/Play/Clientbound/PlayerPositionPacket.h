#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Play::Clientbound {

class PlayerPositionPacket : public Packet<PlayerPositionPacket,
	PID_PLAY_CB_PLAYERPOSITION> {
public:
	PlayerPositionPacket() = default;

	PlayerPositionPacket(int32_t teleportId,
		Vector3 position, Vector3 velocity,
		Vector2 rotation, int32_t flags) {
		m_TeleportId.Value = teleportId;
		m_Position.Value = position;
		m_Velocity.Value = velocity;
		m_Rotation.Value = rotation;
		m_Flags.Value = flags;
	}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	AX_START_FIELDS()
		AX_DECLARE(TeleportId),
		AX_DECLARE(Position),
		AX_DECLARE(Velocity),
		AX_DECLARE(Rotation),
		AX_DECLARE(Flags)
	AX_END_FIELDS()

	AX_FIELD(TeleportId, Net::VarInt)
	AX_FIELD(Position, Net::Vec3)
	AX_FIELD(Velocity, Net::Vec3)
	AX_FIELD(Rotation, Net::Vec2)
	AX_FIELD(Flags, Net::Int)
};

} // namespace Axiom::Play::Clientbound
