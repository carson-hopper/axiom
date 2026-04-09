#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

class LevelTime;

/**
 * Queries or sets the world time of day.
 *
 * Usage: /time [set <value>|day|noon|night|midnight]
 * Permission: server.time
 */
class TimeCommand : public Command {
public:
    const std::string& Name() const override {
        static std::string name = "time";
        return name;
    }

    const std::string& Description() const override {
        static std::string desc = "Set time of day (0-24000 or day/noon/night/midnight)";
        return desc;
    }

    const std::string& Usage() const {
        static std::string usage = "/time [set <value>]";
        return usage;
    }

    const std::string& Permission() const {
        static std::string perm = "axiom.world.time";
        return perm;
    }

    void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

    std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;

private:
    void SetTime(CommandSender& sender, const std::string& value, LevelTime& levelTime);
};

}
