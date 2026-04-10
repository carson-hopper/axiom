#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Grants operator status to a player.
 *
 * Usage: /op <player>
 *
 * Adds the target player to ops.json at the
 * owner level (4) and updates their in-memory
 * op level immediately.
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
