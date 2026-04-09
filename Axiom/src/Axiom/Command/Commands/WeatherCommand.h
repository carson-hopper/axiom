#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Sets the world weather type.
 *
 * Usage: /weather <clear|rain|thunder>
 * Permission: server.weather
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

    const std::string& Usage() const {
        static std::string usage = "/weather <clear|rain|thunder>";
        return usage;
    }

    const std::string& Permission() const {
        static std::string perm = "axiom.world.weather";
        return perm;
    }

    void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

    std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;
};

}
