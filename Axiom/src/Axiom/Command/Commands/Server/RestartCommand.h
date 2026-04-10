#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Saves all chunks and restarts the server process.
 *
 * Usage: /restart
 */
class RestartCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "restart";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Save all chunks and restart the server";
        return desc;
    }

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.server.restart";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 4; }

    Ref<LiteralNode> BuildTree() override;
};

}
