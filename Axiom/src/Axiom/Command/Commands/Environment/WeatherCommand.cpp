#include "axpch.h"
#include "WeatherCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Environment/Level/LevelTime.h"

namespace Axiom {

    namespace {
        int ApplyWeather(CommandSourceStack& source, WeatherType type, const std::string& label) {
            Application::Instance().PacketCtx().Time().SetWeather(type);
            source.SendMessage(ChatComponent::Create()
                .Text("Weather set to " + label)
                .Color(ChatColor::Green)
                .Build());
            return 1;
        }
    }

    Ref<LiteralNode> WeatherCommand::BuildTree() {
        auto root = Literal("weather");
        root->Requires(RequiredPermissionLevel());

        const auto clear = Literal("clear");
        clear->Executes([](CommandSourceStack& source, auto&) {
            return ApplyWeather(source, WeatherType::Clear, "clear");
        });
        root->Then(clear);

        const auto rain = Literal("rain");
        rain->Executes([](CommandSourceStack& source, auto&) {
            return ApplyWeather(source, WeatherType::Rain, "rain");
        });
        root->Then(rain);

        const auto thunder = Literal("thunder");
        thunder->Executes([](CommandSourceStack& source, auto&) {
            return ApplyWeather(source, WeatherType::Thunder, "thunder");
        });
        root->Then(thunder);

        return root;
    }

}
