#include "axpch.h"
#include "Axiom/Command/Commands/WeatherCommand.h"

#include "Axiom/Core/Application.h"
#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Environment/Level/LevelTime.h"

namespace Axiom {

    void WeatherCommand::Execute(CommandSender& sender, const std::vector<std::string>& arguments) {
        if (!sender.HasPermission(Permission())) {
            sender.SendMessage(ChatComponent::Create()
                .Text("You don't have permission to change weather")
                .Color(ChatColor::Red)
                .Build());
            return;
        }

        if (arguments.empty()) {
            sender.SendPlainMessage("Usage: weather <clear|rain|thunder>");
            return;
        }

        const std::string& argument = arguments[0];

        constexpr std::pair<std::string_view, WeatherType> weatherMap[] = {
            {"clear", WeatherType::Clear},
            {"rain", WeatherType::Rain},
            {"thunder", WeatherType::Thunder}
        };

        for (const auto& [name, type] : weatherMap) {
            if (argument == name) {
                Application::Instance().PacketCtx().Time().SetWeather(type);
                sender.SendPlainMessage("Weather set to " + std::string(name));
                return;
            }
        }

        sender.SendPlainMessage("Unknown weather: " + argument);
    }

    std::vector<std::string> WeatherCommand::TabComplete(CommandSender& /*sender*/, const std::vector<std::string>& arguments) {
        if (arguments.size() <= 1) {
            std::vector<std::string> types = {"clear", "rain", "thunder"};
            if (arguments.empty()) {
                return types;
            }
            std::vector<std::string> results;
            for (const auto& type : types) {
                if (type.find(arguments[0]) == 0) {
                    results.push_back(type);
                }
            }
            return results;
        }
        return {};
    }

}
