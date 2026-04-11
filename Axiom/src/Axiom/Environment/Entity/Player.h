#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandSender.h"
#include "Axiom/Core/Observable.h"
#include "Axiom/Core/UUID.h"
#include "Axiom/Environment/Entity/LivingEntity.h"
#include "Axiom/Environment/Level/Level.h"
#include "Axiom/Network/Connection.h"

#include <algorithm>
#include <array>
#include <atomic>
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

		Ref<Level> GetLevel() { return m_Level; }
		void SetLevel(const Ref<Level>& level) { m_Level = level; }

		int GetFoodLevel() const { return m_FoodLevel.Get(); }
		void SetFoodLevel(const int level) { m_FoodLevel.Set(level); }

		float GetSaturationLevel() const { return m_SaturationLevel; }
		void SetSaturationLevel(const float level) { m_SaturationLevel = level; }

		int GetExperienceLevel() const { return m_ExperienceLevel.Get(); }
		void SetExperienceLevel(const int level) { m_ExperienceLevel.Set(level); }

		float GetExperienceProgress() const { return m_ExperienceProgress; }
		void SetExperienceProgress(const float progress) { m_ExperienceProgress = progress; }

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

		/**
		 * Server-authoritative teleport. Updates the
		 * player's position (firing the normal
		 * `PlayerPositionChangedEvent`) AND sends a
		 * `Clientbound::PlayerPositionPacket` to the
		 * owning client so it snaps to the new position.
		 *
		 * Use this for `/tp`, respawn, portal
		 * transitions, and plugin-driven moves — any
		 * case where the server needs the client to
		 * accept a new position regardless of what
		 * inputs the player is currently feeding it.
		 *
		 * Do NOT call from the client-move packet
		 * handlers or from physics — those should use
		 * the plain `SetPosition` setter inherited from
		 * `Entity`, which fires the event without echoing
		 * a sync packet back to the owning client.
		 * Echoing every client-origin move would create
		 * a 20 tps round-trip loop for every player.
		 */
		void Teleport(const Vector3& position);

		/**
		 * Acknowledge the client's `AcceptTeleportation`
		 * echo for a pending server-issued teleport.
		 *
		 * On match: clears the pending slot and commits
		 * the current authoritative position as the new
		 * "last known good" snapshot — the server now
		 * trusts that the client has caught up.
		 *
		 * On mismatch: rolls the authoritative position
		 * back to `m_LastGoodPosition` via a plain
		 * `SetPosition` (no new sync packet, so no
		 * packet-loop risk if the client keeps mis-
		 * acking). The client will self-correct on its
		 * next move packet when the move validator's
		 * delta cap snaps it back.
		 */
		void ConfirmTeleport(int32_t teleportId);

		/**
		 * Current outstanding teleport id that the
		 * server has sent but the client has not yet
		 * echoed back. Returns 0 when no teleport is
		 * pending.
		 */
		int32_t PendingTeleportId() const {
			return m_PendingTeleportId.load(std::memory_order_acquire);
		}

		/**
		 * True while the server is waiting for the
		 * client to echo `AcceptTeleportation` for the
		 * most recent forced teleport. Move-packet
		 * handlers check this and drop the packet
		 * silently, because any position the client
		 * reports before it has applied the teleport
		 * is by definition stale.
		 */
		bool IsAwaitingTeleportAck() const {
			return PendingTeleportId() != 0;
		}

		/**
		 * Stateless positional sanity check. Returns
		 * `true` when `position` is finite, inside the
		 * ±30 million world border on X/Z, and inside
		 * the [-512, 1024] Y envelope.
		 *
		 * Shared between `IsValidMoveTarget` (which adds
		 * the per-tick delta cap on top) and `Teleport`
		 * (which does NOT cap delta — teleports are
		 * supposed to jump). Anything that mutates the
		 * authoritative player position MUST route
		 * through one of those two gates.
		 */
		static bool IsValidPosition(const Vector3& position);

		/**
		 * Server-side validation of a client-reported
		 * position. Returns `false` when the target
		 * must be rejected and the client snapped back
		 * via `Teleport(GetPosition())`.
		 *
		 * Checks in order:
		 *  - Everything `IsValidPosition` checks
		 *  - The squared single-tick delta from the
		 *    current authoritative position is at most
		 *    100² blocks (matches vanilla's "moved too
		 *    quickly" threshold, which covers walk,
		 *    sprint, creative fly, elytra, and riptide)
		 *
		 * Future work: make the delta cap gamemode- and
		 * vehicle- and effect-aware, and fold in a
		 * multi-tick accumulated-speed check for smoother
		 * anti-cheat on fly / speed hacks.
		 */
		bool IsValidMoveTarget(const Vector3& position) const;

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
		Ref<Level> m_Level;
		std::string m_Name;
		UUID m_Uuid;
		std::vector<PlayerProperty> m_Properties;
		std::unordered_set<std::string> m_GrantedPermissions;
		Observable<OpLevel> m_OpLevel{OpLevel::None};
		Observable<GameMode> m_GameMode{GameMode::Creative};
		Observable<int> m_SelectedSlot{0};
		std::array<HotbarSlot, 9> m_HotbarItems{};
		Observable<int> m_FoodLevel{20};
		float m_SaturationLevel = 5.0f;
		Observable<int> m_ExperienceLevel{0};
		float m_ExperienceProgress = 0.0f;
		int m_TotalExperience = 0;

		/**
		 * Monotonic per-player teleport id used when
		 * the server issues `Clientbound::PlayerPosition`
		 * to force a client-side sync (e.g. from /tp or
		 * respawn). Vanilla clients echo the id back in
		 * `ServerboundAcceptTeleportation` so the server
		 * can ignore stale client moves sent before the
		 * teleport landed. Starts at 1 so 0 stays
		 * reserved for "no teleport pending".
		 */
		std::atomic<int32_t> m_NextTeleportId{1};

		/**
		 * The id of the most recent teleport the server
		 * has sent but the client has not yet echoed
		 * back via `AcceptTeleportation`. 0 means no
		 * teleport is pending. `Teleport()` overwrites
		 * this with the new id on every dispatch, and
		 * `ConfirmTeleport()` clears it to 0 when the
		 * client echoes a matching id.
		 */
		std::atomic<int32_t> m_PendingTeleportId{0};

		/**
		 * Last authoritative position the client has
		 * been confirmed to agree with. Captured at the
		 * start of the first `Teleport` in a pending
		 * chain (when `m_PendingTeleportId == 0`), so
		 * subsequent teleports within the same chain do
		 * not clobber the rollback target. Committed as
		 * the current position on a successful
		 * `ConfirmTeleport`. Used as the revert target
		 * on a mismatched ack.
		 */
		Vector3 m_LastGoodPosition;
	};

}
