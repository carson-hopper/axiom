#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Sets the world weather type.
 *
 * Usage: /weather <clear|rain|thunder>
 */
class WeatherCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "weather";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Set weather (clear/rain/thunder)";
        return desc;
    }

	const std::string& RequiredPermission() const override {
    	static constexpr std::string perm = "axiom.world.weather";
    	return perm;
    }

    int RequiredPermissionLevel() const override { return 2; }

    Ref<LiteralNode> BuildTree() override;
};

}
