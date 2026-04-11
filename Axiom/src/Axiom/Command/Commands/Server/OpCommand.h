#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Grants operator status to a player.
 *
 * Usage: /op <player> [level]
 *
 * Adds the target player to ops.toml and
 * updates their in-memory op level via the
 * Player::m_OpLevel Observable, which fires
 * a PlayerOperatorLevelChangeEvent.
 *
 * When run by a player, the default grant
 * is one tier below the sender's own level
 * (source.level - 1), and the explicit
 * [level] form is rejected. Only the console
 * may pick an arbitrary level 1-4; its
 * default is Owner (4).
 *
 * Escalation rule: a player may only op a
 * target whose current op level is strictly
 * below their own. The console bypasses the
 * escalation check.
 */
class OpCommand : public Command {
public:
	const std::string& Name() const override {
		static std::string name = "op";
		return name;
	}

	const std::string& Description() const override {
		static std::string desc = "Grants operator status to a player";
		return desc;
	}

	int RequiredPermissionLevel() const override { return 3; }

	Ref<LiteralNode> BuildTree() override;
};

}
