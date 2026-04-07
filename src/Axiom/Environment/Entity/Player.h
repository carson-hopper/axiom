#pragma once

#include "Axiom/Command/CommandSender.h"
#include "Axiom/Environment/Entity/LivingEntity.h"
#include "Axiom/Environment/Level/Level.h"
#include "Axiom/Network/Connection.h"

#include <algorithm>
#include <array>
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
	class Player : public LivingEntity, public CommandSender {
	public:
		Player(const int32_t entityId, Ref<Connection> connection,
			std::string  name, std::string  uuid)
			: LivingEntity(entityId)
			, m_Connection(std::move(connection))
			, m_Name(std::move(name))
			, m_Uuid(std::move(uuid)) {}

		const std::string& Name() const override { return m_Name; }
		const std::string& Uuid() const { return m_Uuid; }
		Ref<Connection> GetConnection() const { return m_Connection; }
		GameMode GetGameMode() const { return m_GameMode; }
		void SetGameMode(const GameMode mode) { m_GameMode = mode; }

		Ref<class Level> GetLevel() const { return m_Level.lock(); }
		void SetLevel(const Ref<class Level>& level) { m_Level = level; }

		int GetFoodLevel() const { return m_FoodLevel; }
		void SetFoodLevel(const int level) { m_FoodLevel = level; }

		float GetSaturationLevel() const { return m_SaturationLevel; }
		void SetSaturationLevel(const float level) { m_SaturationLevel = level; }

		int GetExperienceLevel() const { return m_ExperienceLevel; }
		void SetExperienceLevel(const int level) { m_ExperienceLevel = level; }

		float GetExperienceProgress() const { return m_ExperienceProgress; }
		void SetExperienceProgress(float progress) { m_ExperienceProgress = progress; }

		int GetTotalExperience() const { return m_TotalExperience; }
		void SetTotalExperience(const int total) { m_TotalExperience = total; }

		// ----- Inventory ------------------------------------------------

		int GetSelectedSlot() const { return m_SelectedSlot; }
		void SetSelectedSlot(const int slot) { m_SelectedSlot = std::clamp(slot, 0, 8); }

		int32_t GetHeldItemId() const { return m_HotbarItems[m_SelectedSlot].itemId; }

		struct HotbarSlot {
			int32_t itemId = 0;
			int32_t count = 0;
		};

		void SetHotbarItem(const int slot, const int32_t itemId, const int32_t count) {
			if (slot >= 0 && slot < 9) {
				m_HotbarItems[slot] = {itemId, count};
			}
		}

		// ----- Actions --------------------------------------------------

		void SendMessage(const std::string& message) override;
		void Kick(const std::string& reason = "") const;

		bool IsPlayer() const override { return true; }
		bool HasPermission(const std::string& /*permission*/) const override { return true; }

	protected:
		void OnDeath() override;

	private:
		Ref<Connection> m_Connection;
		std::weak_ptr<Level> m_Level;
		std::string m_Name;
		std::string m_Uuid;
		GameMode m_GameMode = GameMode::Spectator;
		int m_SelectedSlot = 0;
		std::array<HotbarSlot, 9> m_HotbarItems{};
		int m_FoodLevel = 20;
		float m_SaturationLevel = 5.0f;
		int m_ExperienceLevel = 0;
		float m_ExperienceProgress = 0.0f;
		int m_TotalExperience = 0;
	};

}
