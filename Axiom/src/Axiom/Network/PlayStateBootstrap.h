#pragma once

#include "Axiom/Core/Base.h"

namespace Axiom {

	class Connection;
	class PacketContext;

	/**
	 * Encapsulates the full play-state join sequence
	 * for a player transitioning from Configuration
	 * to Play state.
	 */
	class PlayStateBootstrap {
	public:
		/**
		 * Execute the complete play-state bootstrap
		 * for a connecting player. Called after
		 * Configuration -> Play transition.
		 */
		static void Bootstrap(
			const Ref<Connection>& connection,
			PacketContext& context);

	private:
		/**
		 * Send JoinGame/Login packet with dimension,
		 * seed, game mode, view distance.
		 */
		static void SendLoginPacket(
			const Ref<Connection>& connection,
			PacketContext& context);

		/**
		 * Send player abilities for
		 * creative mode (fly, invulnerable,
		 * instant break).
		 */
		static void SendPlayerAbilities(
			const Ref<Connection>& connection);

		/**
		 * Send default spawn position
		 * in the overworld dimension.
		 */
		static void SendSpawnPosition(
			const Ref<Connection>& connection,
			double spawnY);

		/**
		 * Send initial player position and
		 * velocity after joining.
		 */
		static void SendPlayerPosition(
			const Ref<Connection>& connection,
			double spawnY);

		/**
		 * Queue initial chunk delivery
		 * to the connecting client.
		 */
		static void SendInitialChunks(
			const Ref<Connection>& connection,
			PacketContext& context,
			double spawnY);

		/**
		 * Send a game event packet
		 * to the client.
		 */
		static void SendGameEvent(
			const Ref<Connection>& connection,
			uint8_t eventId,
			float value);
	};

}
