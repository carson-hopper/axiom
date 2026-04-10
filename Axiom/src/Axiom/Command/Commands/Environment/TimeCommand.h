#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Queries or sets the world time of day.
 *
 * Usage: /time [set <value>|set day|set noon|set sunset|set night|set midnight]
 */
class TimeCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "time";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Set time of day (0-24000 or day/noon/sunset/night/midnight)";
        return desc;
    }

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.world.time";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 2; }

    Ref<LiteralNode> BuildTree() override;
};

}
