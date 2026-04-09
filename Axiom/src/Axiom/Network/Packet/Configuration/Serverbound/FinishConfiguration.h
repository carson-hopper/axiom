#pragma once

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Config/ServerConfig.h"
#include "Axiom/Network/Packet/Play/Clientbound/LoginPacket.h"
#include "Axiom/Network/Packet/Play/Clientbound/PlayerInfoUpdate.h"
#include "Axiom/Network/Packet/Play/Clientbound/PlayerAbilitiesPacket.h"
#include "Axiom/Network/Packet/Play/Clientbound/SetDefaultSpawnPositionPacket.h"
#include "Axiom/Network/Packet/Play/Clientbound/PlayerPositionPacket.h"
#include "Axiom/Network/Packet/Play/Clientbound/GameEventPacket.h"

namespace Axiom::Configuration::Serverbound {

/**
 * GameEvent that triggers chunk delivery after being sent.
 * Ensures Login packet has created the client world
 * before SetChunkCacheCenter arrives.
 */
class ChunkLoadStartEvent : public Play::Clientbound::GameEventPacket {
public:
	ChunkLoadStartEvent(PacketContext& context, Vector3 playerPos)
		: GameEventPacket(Play::Clientbound::GameEventType::LevelChunksLoadStart)
		, m_Context(context)
		, m_PlayerPos(playerPos) {}

	void OnSent(const Ref<Connection>& connection) override {
		m_Context.ChunkManagement().SendInitialChunks(
			connection, m_PlayerPos.x, m_PlayerPos.z);
	}

private:
	PacketContext& m_Context;
	Vector3 m_PlayerPos;
};

class FinishConfigurationPacket : public Packet<FinishConfigurationPacket,
	PID_CONFIGURATION_SB_FINISHCONFIGURATION> {
public:
	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>& connection, PacketContext& context, NetworkBuffer&) {
		AX_CORE_INFO("Configuration complete for {}", connection->RemoteAddress());
		connection->State(ConnectionState::Play);

		// Get or create the player
		auto player = context.Server().GetPlayer(connection->Id());
		if (!player) {
			player = context.Server().AddPlayer(connection, "Player", UUID{});
			player->SetPosition({0.5, context.ChunkManagement().Generator().SpawnY(), 0.5});
			player->SetGameMode(GameMode::Creative);
		}

		std::vector<Ref<IChainablePacket>> chain;

		// 1. Login/JoinGame — from player + config
		chain.push_back(CreateRef<Play::Clientbound::LoginPacket>(
			player->GetEntityId(),
			context.Config().MaxPlayers(),
			context.Config().ViewDistance(),
			context.Config().SimulationDistance(),
			static_cast<uint8_t>(player->GetGameMode()),
			63));

		// 2. Player info (skin textures) — all online players
		auto infoPacket = CreateRef<Play::Clientbound::PlayerInfoUpdatePacket>();
		infoPacket->AddPlayer(player);
		for (const auto& other : context.Server().AllPlayers()) {
			if (other->GetConnection()->Id() != connection->Id())
				infoPacket->AddPlayer(other);
		}
		chain.push_back(infoPacket);

		// 3. Player abilities — from game mode
		uint8_t abilityFlags = 0;
		if (player->GetGameMode() == GameMode::Creative) {
			abilityFlags = 0x01 | 0x04 | 0x08; // invulnerable + allow fly + instant break
		} else if (player->GetGameMode() == GameMode::Spectator) {
			abilityFlags = 0x01 | 0x02 | 0x04; // invulnerable + flying + allow fly
		}
		chain.push_back(CreateRef<Play::Clientbound::PlayerAbilitiesPacket>(
			static_cast<int8_t>(abilityFlags), 0.05f, 0.1f));

		// 4. Spawn position — from player position
		auto& spawnPos = player->GetPosition();
		chain.push_back(CreateRef<Play::Clientbound::SetDefaultSpawnPositionPacket>(
			"minecraft:overworld",
			static_cast<int32_t>(spawnPos.x),
			static_cast<int32_t>(spawnPos.y),
			static_cast<int32_t>(spawnPos.z),
			player->GetRotation().x,
			player->GetRotation().y));

		// 5. Player position — from player entity
		chain.push_back(CreateRef<Play::Clientbound::PlayerPositionPacket>(
			0,
			player->GetPosition(),
			player->GetVelocity(),
			player->GetRotation(),
			0));

		// 6. Start waiting for chunks — OnSent triggers chunk delivery
		//    after the client has processed Login and created the world
		chain.push_back(CreateRef<ChunkLoadStartEvent>(
			context, player->GetPosition()));

		// Broadcast new player to existing players
		auto broadcastPacket = CreateRef<Play::Clientbound::PlayerInfoUpdatePacket>();
		broadcastPacket->AddPlayer(player);
		NetworkBuffer broadcastPayload;
		broadcastPacket->Write(broadcastPayload);
		for (const auto& other : context.Server().AllPlayers()) {
			if (other->GetConnection()->Id() != connection->Id()
				&& other->GetConnection()->IsConnected()) {
				other->GetConnection()->SendRawPacket(
					broadcastPacket->GetPacketId(), broadcastPayload);
			}
		}

		AX_CORE_INFO("{} joined the world (entity {})", player->Name(), player->GetEntityId());
		return chain;
	}

	AX_START_FIELDS()
	AX_END_FIELDS()
};

} // namespace Axiom::Configuration::Serverbound
