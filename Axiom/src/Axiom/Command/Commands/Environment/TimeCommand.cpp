#include "axpch.h"
#include "TimeCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Environment/Level/LevelTime.h"

#include <stdexcept>
#include <string>

namespace Axiom {

    namespace {
        int ApplyPreset(CommandSourceStack& source, int64_t ticks) {
            Application::Instance().PacketCtx().Time().SetTimeOfDay(ticks);
            source.SendMessage(ChatComponent::Create()
                .Text("Set time to " + std::to_string(ticks))
                .Color(ChatColor::Green)
                .Build());
            return 1;
        }

        Ref<LiteralNode> MakePreset(const std::string& name, int64_t ticks) {
            auto preset = Literal(name);
            preset->Executes([ticks](CommandSourceStack& source, auto&) {
                return ApplyPreset(source, ticks);
            });
            return preset;
        }
    }

    Ref<LiteralNode> TimeCommand::BuildTree() {
        auto root = Literal("time");
        root->Requires(RequiredPermissionLevel());

        root->Executes([](CommandSourceStack& source, auto&) {
            const auto& levelTime = Application::Instance().PacketCtx().Time();
            source.SendMessage(ChatComponent::Create()
                .Text("Time: " + std::to_string(levelTime.TimeOfDay()))
                .Color(ChatColor::White)
                .Build());
            return 1;
        });

        const auto set = Literal("set");

        set->Then(MakePreset("day", 1000));
        set->Then(MakePreset("noon", 6000));
        set->Then(MakePreset("sunset", 12000));
        set->Then(MakePreset("night", 13000));
        set->Then(MakePreset("midnight", 18000));

        const auto value = Argument("value", CreateRef<IntegerParser>(0));
        value->Executes([](CommandSourceStack& source, const std::unordered_map<std::string, std::string>& arguments) {
            const auto iterator = arguments.find("value");
            if (iterator == arguments.end()) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Missing time value")
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }
            int64_t ticks = 0;
            try {
                ticks = std::stoll(iterator->second);
            } catch (const std::exception&) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Invalid time: " + iterator->second)
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }
            if (ticks < 0) {
                source.SendFailure(ChatComponent::Create()
                    .Text("Time must be non-negative")
                    .Color(ChatColor::Red)
                    .Build());
                return 0;
            }
            return ApplyPreset(source, ticks);
        });
        set->Then(value);

        root->Then(set);

        return root;
    }

}
