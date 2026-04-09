#pragma once

#include "Axiom/Network/Packet/Packet.h"

namespace Axiom::Play::Clientbound {

enum class GameEventType : uint8_t {
	NoRespawnBlockAvailable,
	StartRaining,
	StopRaining,
	ChangeGameMode,
	WinGame,
	DemoEvent,
	PlayArrowHitSound,
	RainLevelChange,
	ThunderLevelChange,
	PufferFishSting,
	GuardianElderEffect,
	ImmediateRespawn,
	LimitedCrafting,
	LevelChunksLoadStart,
};

class GameEventPacket : public Packet<GameEventPacket,
	PID_PLAY_CB_GAMEEVENT> {
public:
	GameEventPacket() = default;

	GameEventPacket(GameEventType event, float value = 0.0f) {
		m_EventId.Value = static_cast<uint8_t>(event);
		m_Value.Value = value;
	}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	AX_START_FIELDS()
		AX_DECLARE(EventId),
		AX_DECLARE(Value)
	AX_END_FIELDS()

	AX_FIELD(EventId, Net::UnsignedByte)
	AX_FIELD(Value, Net::Float)
};

} // namespace Axiom::Play::Clientbound
