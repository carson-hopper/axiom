#pragma once

#include "Axiom/Environment/Entity/LivingEntity.h"
#include "Axiom/Network/Connection.h"

#include <memory>
#include <string>
#include <utility>

namespace Axiom {

	enum class GameMode {
		Survival = 0,
		Creative = 1,
		Adventure = 2,
		Spectator = 3
	};

	/**
	 * Represents a connected player in the world.
	 * Links a LivingEntity to a network Connection and holds
	 * player-specific state (name, UUID, game mode, etc.)
	 */
	class Player : public LivingEntity {
	public:
		Player(const int32_t entityId, Ref<Connection> connection,
			std::string  name, std::string  uuid)
			: LivingEntity(entityId)
			, m_Connection(std::move(connection))
			, m_Name(std::move(name))
			, m_Uuid(std::move(uuid)) {}

		const std::string& Name() const { return m_Name; }
		const std::string& Uuid() const { return m_Uuid; }
		Ref<Connection> GetConnection() const { return m_Connection; }
		GameMode GetGameMode() const { return m_GameMode; }
		void SetGameMode(const GameMode mode) { m_GameMode = mode; }

		Ref<class World> GetWorld() const { return m_World.lock(); }
		void SetWorld(const Ref<class World>& world) { m_World = world; }

		int FoodLevel() const { return m_FoodLevel; }
		void SetFoodLevel(const int level) { m_FoodLevel = level; }

		float SaturationLevel() const { return m_SaturationLevel; }
		void SetSaturationLevel(const float level) { m_SaturationLevel = level; }

		int ExperienceLevel() const { return m_ExperienceLevel; }
		void SetExperienceLevel(const int level) { m_ExperienceLevel = level; }

		float ExperienceProgress() const { return m_ExperienceProgress; }
		void SetExperienceProgress(float progress) { m_ExperienceProgress = progress; }

		int TotalExperience() const { return m_TotalExperience; }
		void SetTotalExperience(const int total) { m_TotalExperience = total; }

		/**
		 * Send a chat message to this player.
		 */
		void SendMessage(const std::string& message);

		/**
		 * Disconnect this player with an optional reason.
		 */
		void Kick(const std::string& reason = "");

	protected:
		void OnDeath() override;

	private:
		Ref<Connection> m_Connection;
		std::weak_ptr<class World> m_World;
		std::string m_Name;
		std::string m_Uuid;
		GameMode m_GameMode = GameMode::Creative;
		int m_FoodLevel = 20;
		float m_SaturationLevel = 5.0f;
		int m_ExperienceLevel = 0;
		float m_ExperienceProgress = 0.0f;
		int m_TotalExperience = 0;
	};

}
