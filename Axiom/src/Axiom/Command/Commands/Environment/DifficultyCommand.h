#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Queries or changes the server difficulty level.
 *
 * Usage: /difficulty [peaceful|easy|normal|hard]
 */
class DifficultyCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "difficulty";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Get or set the difficulty level";
        return desc;
    }

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.world.difficulty";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 2; }

    Ref<LiteralNode> BuildTree() override;
};

}
