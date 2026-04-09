#include "axpch.h"
#include "Axiom/Command/Commands/TimeCommand.h"

#include "Axiom/Core/Application.h"
#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Environment/Level/LevelTime.h"

#include <charconv>

namespace Axiom {

    void TimeCommand::Execute(CommandSender& sender, const std::vector<std::string>& arguments) {
        if (!sender.HasPermission(Permission())) {
            sender.SendMessage(ChatComponent::Create()
                .Text("You don't have permission to change time")
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        auto& levelTime = Application::Instance().PacketCtx().Time();

        if (arguments.empty()) {
            sender.SendPlainMessage("Time: " + std::to_string(levelTime.TimeOfDay()));
            return;
        }

        const std::string& argument = arguments[0];

        // Handle "time set <value>" subcommand
        if (argument == "set" && arguments.size() >= 2) {
            SetTime(sender, arguments[1], levelTime);
            return;
        }

        // Handle named presets and numeric values directly
        SetTime(sender, argument, levelTime);
    }

    void TimeCommand::SetTime(CommandSender& sender, const std::string& value, LevelTime& levelTime) {
        constexpr std::pair<std::string_view, int64_t> timeMap[] = {
            {"day", 1000},
            {"noon", 6000},
            {"sunset", 12000},
            {"night", 13000},
            {"midnight", 18000}
        };

        for (const auto& [name, ticks] : timeMap) {
            if (value == name) {
                levelTime.SetTimeOfDay(ticks);
                sender.SendPlainMessage("Set time to " + std::to_string(ticks));
                return;
            }
        }

        int64_t parsed;
        auto [pointer, errorCode] = std::from_chars(
            value.data(), value.data() + value.size(), parsed);

        if (errorCode != std::errc() || pointer != value.data() + value.size()) {
            sender.SendPlainMessage("Invalid time: " + value);
            return;
        }

        if (parsed < 0 || parsed > 24000) {
            sender.SendPlainMessage("Time must be between 0 and 24000");
            return;
        }

        levelTime.SetTimeOfDay(parsed);
        sender.SendPlainMessage("Set time to " + std::to_string(parsed));
    }

    std::vector<std::string> TimeCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& arguments) {
        if (arguments.size() <= 1) {
            std::vector<std::string> options = {"set", "day", "noon", "sunset", "night", "midnight"};
            if (arguments.empty()) {
                return options;
            }
            std::vector<std::string> results;
            for (const auto& option : options) {
                if (option.find(arguments[0]) == 0) {
                    results.push_back(option);
                }
            }
            return results;
        }
        if (arguments.size() == 2 && arguments[0] == "set") {
            std::vector<std::string> presets = {"day", "noon", "sunset", "night", "midnight"};
            std::vector<std::string> results;
            for (const auto& preset : presets) {
                if (preset.find(arguments[1]) == 0) {
                    results.push_back(preset);
                }
            }
            return results;
        }
        return {};
    }

}
