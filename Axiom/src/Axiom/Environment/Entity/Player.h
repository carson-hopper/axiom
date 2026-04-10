#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandSender.h"
#include "Axiom/Core/Observable.h"
#include "Axiom/Core/UUID.h"
#include "Axiom/Environment/Entity/LivingEntity.h"
#include "Axiom/Environment/Level/Level.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Utilities/Memory/WeakRef.h"

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

namespace Axiom {

	enum class GameMode {
		Survival = 0,
		Creative = 1,
		Adventure = 2,
		Spectator = 3
	};

	/**
	 * Vanilla operator permission levels.
	 * Values map 1:1 onto the 0..4 integer
	 * levels used by the command dispatcher.
	 */
	enum class OpLevel {
		None = 0,                  /** Not an op, no privileges    */
		BypassSpawnProtection = 1, /** Can bypass spawn protection */
		Moderator = 2,             /** Default command access      */
		Admin = 3,                 /** Player management commands  */
		Owner = 4                  /** All commands including stop */
	};

	/**
	 * Represents a connected player in the world.
	 * Links a LivingEntity to a network Connection and holds
	 * player-specific state (name, UUID, game mode, etc.)
	 */
	struct PlayerProperty {
		std::string Name;
		std::string Value;
		std::string Signature; // empty if unsigned
	};

	class Player : public LivingEntity, public CommandSender {
	public:
		Player(Ref<Connection> connection,
			std::string name, const UUID& uuid);

		const std::string& Name() const override { return m_Name; }
		const UUID& GetUuid() const { return m_Uuid; }

		const std::vector<PlayerProperty>& Properties() const { return m_Properties; }
		void SetProperties(std::vector<PlayerProperty> properties) { m_Properties = std::move(properties); }
		Ref<Connection> GetConnection() const { return m_Connection; }
		GameMode GetGameMode() const { return m_GameMode.Get(); }
		void SetGameMode(const GameMode mode) { m_GameMode.Set(mode); }

		Ref<Level> GetLevel() { return m_Level.IsValid() ? Ref<Level>(&(*m_Level)) : nullptr; }
		void SetLevel(const Ref<Level>& level) { m_Level = WeakRef<Level>(level); }

		int GetFoodLevel() const { return m_FoodLevel.Get(); }
		void SetFoodLevel(const int level) { m_FoodLevel.Set(level); }

		float GetSaturationLevel() const { return m_SaturationLevel; }
		void SetSaturationLevel(const float level) { m_SaturationLevel = level; }

		int GetExperienceLevel() const { return m_ExperienceLevel.Get(); }
		void SetExperienceLevel(const int level) { m_ExperienceLevel.Set(level); }

		float GetExperienceProgress() const { return m_ExperienceProgress; }
		void SetExperienceProgress(float progress) { m_ExperienceProgress = progress; }

		int GetTotalExperience() const { return m_TotalExperience; }
		void SetTotalExperience(const int total) { m_TotalExperience = total; }

		// ----- Inventory ------------------------------------------------

		int GetSelectedSlot() const { return m_SelectedSlot.Get(); }
		void SetSelectedSlot(const int slot) {
			m_SelectedSlot.Set(std::clamp(slot, 0, 8));
		}

		int32_t GetHeldItemId() const { return m_HotbarItems[m_SelectedSlot.Get()].itemId; }

		struct HotbarSlot {
			int32_t itemId = 0;
			int32_t count = 0;
		};

		void SetHotbarItem(const int slot, const int32_t itemId, const int32_t count) {
			if (slot >= 0 && slot < 9) {
				m_HotbarItems[slot] = {itemId, count};
			}
		}

		// ----- Observable accessors -------------------------------------

		Observable<GameMode>& GameModeObservable() { return m_GameMode; }
		Observable<int>& FoodObservable() { return m_FoodLevel; }
		Observable<int>& ExperienceObservable() { return m_ExperienceLevel; }
		Observable<int>& SelectedSlotObservable() { return m_SelectedSlot; }

		// ----- Actions --------------------------------------------------

		void SendMessage(const ChatText& message) override;
		void Kick(const ChatText& reason = {}) const;

		bool IsPlayer() const override { return true; }

		// ----- Permissions ----------------------------------------------

		/**
		 * Check whether this player has the given permission.
		 * Grants match via MatchesPermissionPattern, so wildcards
		 * like "axiom.gamemode.*" work.
		 */
		bool HasPermission(const std::string& permission) const override;

		/**
		 * Grant a permission (exact or wildcard pattern).
		 */
		void GrantPermission(const std::string& permission) {
			m_GrantedPermissions.insert(permission);
		}

		/**
		 * Revoke a previously granted permission.
		 */
		void RevokePermission(const std::string& permission) {
			m_GrantedPermissions.erase(permission);
		}

		/**
		 * Returns the set of granted permission patterns.
		 */
		const std::unordered_set<std::string>& GrantedPermissions() const {
			return m_GrantedPermissions;
		}

		/**
		 * Get the vanilla op level for this player.
		 * Ranges from None through Owner, matching
		 * Notchian semantics.
		 */
		OpLevel GetOpLevel() const { return m_OpLevel; }

		/**
		 * Set the vanilla op level.
		 */
		void SetOpLevel(const OpLevel level) { m_OpLevel = level; }

	protected:
		void OnDeath() override;

	private:
		Ref<Connection> m_Connection;
		WeakRef<Level> m_Level;
		std::string m_Name;
		UUID m_Uuid;
		std::vector<PlayerProperty> m_Properties;
		std::unordered_set<std::string> m_GrantedPermissions;
		OpLevel m_OpLevel = OpLevel::None;
		Observable<GameMode> m_GameMode{GameMode::Creative};
		Observable<int> m_SelectedSlot{0};
		std::array<HotbarSlot, 9> m_HotbarItems{};
		Observable<int> m_FoodLevel{20};
		float m_SaturationLevel = 5.0f;
		Observable<int> m_ExperienceLevel{0};
		float m_ExperienceProgress = 0.0f;
		int m_TotalExperience = 0;
	};

}
