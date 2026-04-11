#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Lists, queries, or sets game rules.
 *
 * Usage: /gamerule [rule] [value]
 */
class GameruleCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "gamerule";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "List, query, or set game rules";
        return desc;
    }

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.world.gamerule";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 2; }

    Ref<LiteralNode> BuildTree() override;
};

}
