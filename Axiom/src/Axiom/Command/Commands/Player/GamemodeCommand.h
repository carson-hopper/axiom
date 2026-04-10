#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Changes the game mode for a player.
 *
 * Usage: /gamemode <survival|creative|adventure|spectator>
 */
class GamemodeCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "gamemode";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Set the game mode";
        return desc;
    }

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.player.gamemode";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 2; }

    Ref<LiteralNode> BuildTree() override;
};

}
