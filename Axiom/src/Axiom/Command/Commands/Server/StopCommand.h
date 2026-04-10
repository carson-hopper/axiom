#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Stops the server gracefully.
 *
 * Usage: /stop
 */
class StopCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "stop";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Stop the server";
        return desc;
    }

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.server.stop";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 4; }

    Ref<LiteralNode> BuildTree() override;
};

}
