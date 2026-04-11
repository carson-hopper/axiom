#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Revokes operator status from a player.
 *
 * Usage: /deop <player>
 *
 * Removes the target player from ops.toml
 * and resets their in-memory op level to
 * None via the Player::m_OpLevel Observable,
 * which fires a PlayerOperatorLevelChangeEvent.
 *
 * Escalation rule: a player may only deop a
 * target whose current op level is strictly
 * below their own. The console bypasses the
 * escalation check.
 */
class DeopCommand : public Command {
public:
	const std::string& Name() const override {
		static std::string name = "deop";
		return name;
	}

	const std::string& Description() const override {
		static std::string desc = "Revokes operator status from a player";
		return desc;
	}

	int RequiredPermissionLevel() const override { return 3; }

	Ref<LiteralNode> BuildTree() override;
};

}
