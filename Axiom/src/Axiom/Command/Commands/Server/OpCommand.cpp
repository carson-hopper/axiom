#include "axpch.h"
#include "Axiom/Command/Commands/Server/OpCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Config/AdminFileStore.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Environment/Entity/Player.h"
#include "Axiom/Network/Packet/Play/Clientbound/Commands.h"

#include <stdexcept>
#include <string>

namespace Axiom {

	namespace {
		int ApplyOp(CommandSourceStack& source,
			const std::string& targetName, const int newLevel) {

			auto& application = Application::Instance();
			auto& server = application.PacketCtx().Server();
			auto& adminFiles = application.AdminFiles();

			auto target = server.GetPlayerByName(targetName);
			if (!target) {
				source.SendFailure(ChatComponent::Create()
					.Text("Player '" + targetName + "' is not online")
					.Color(ChatColor::Red)
					.Build());
				return 0;
			}

			// Prevent overriding ops with a higher
			// current level. Console bypasses.
			if (source.IsPlayer()) {
				const int sourceLevel = source.GetPermissionLevel();
				const int currentLevel = static_cast<int>(target->GetOpLevel());
				if (currentLevel >= sourceLevel) {
					source.SendFailure(ChatComponent::Create()
						.Text("You cannot op " + targetName
							+ " because their current op level is not below yours")
						.Color(ChatColor::Red)
						.Build());
					return 0;
				}
			}

			const int currentLevel = static_cast<int>(target->GetOpLevel());
			if (adminFiles.IsOp(target->GetUuid().ToString())
				&& currentLevel == newLevel) {
				source.SendFailure(ChatComponent::Create()
					.Text(targetName + " is already an operator at level "
						+ std::to_string(newLevel))
					.Color(ChatColor::Red)
					.Build());
				return 0;
			}

			OpEntry entry;
			entry.Uuid = target->GetUuid().ToString();
			entry.Level = newLevel;
			entry.BypassesPlayerLimit = false;
			adminFiles.AddOp(entry);

			target->SetOpLevel(static_cast<OpLevel>(newLevel));

			// Resend the command graph so the
			// target immediately sees any newly
			// permitted commands in their client.
			auto refreshPacket = Ref<Play::Clientbound::CommandsPacket>::Create(
				application.Commands().GetRootNodes(), target);
			NetworkBuffer payload;
			refreshPacket->Write(payload);
			target->GetConnection()->SendRawPacket(
				refreshPacket->GetPacketId(), payload);

			source.SendMessage(ChatComponent::Create()
				.Text("Made " + targetName + " a server operator at level "
					+ std::to_string(newLevel))
				.Color(ChatColor::Green)
				.Build());

			target->SendMessage(ChatComponent::Create()
				.Text("You are now a server operator at level "
					+ std::to_string(newLevel))
				.Color(ChatColor::Yellow)
				.Build());

			return 1;
		}
	}

	Ref<LiteralNode> OpCommand::BuildTree() {
		auto root = Literal("op");
		root->Requires(RequiredPermissionLevel());
		root->Requires(RequiredPermission());

		const auto target = Argument("target", Ref<EntityParser>::Create(true, true));
		target->Executes([](CommandSourceStack& source,
			const std::unordered_map<std::string, std::string>& arguments) {

			const auto iterator = arguments.find("target");
			if (iterator == arguments.end() || iterator->second.empty()) {
				source.SendFailure(ChatComponent::Create()
					.Text("Missing target player")
					.Color(ChatColor::Red)
					.Build());
				return 0;
			}

			// Default level: console grants Owner,
			// players grant one level below their own.
			int newLevel = 0;
			if (source.IsPlayer()) {
				newLevel = source.GetPermissionLevel() - 1;
				if (newLevel < 1) {
					source.SendFailure(ChatComponent::Create()
						.Text("Your op level is too low to promote others")
						.Color(ChatColor::Red)
						.Build());
					return 0;
				}
			} else {
				newLevel = static_cast<int>(OpLevel::Owner);
			}

			return ApplyOp(source, iterator->second, newLevel);
		});

		const auto levelArgument = Argument("level", Ref<IntegerParser>::Create(1, 4));
		levelArgument->Executes([](CommandSourceStack& source,
			const std::unordered_map<std::string, std::string>& arguments) {

			// Only the console can specify an
			// explicit op level; players use
			// the default grant instead.
			if (source.IsPlayer()) {
				source.SendFailure(ChatComponent::Create()
					.Text("Only the console can specify an op level")
					.Color(ChatColor::Red)
					.Build());
				return 0;
			}

			const auto targetIterator = arguments.find("target");
			const auto levelIterator = arguments.find("level");
			if (targetIterator == arguments.end()
				|| levelIterator == arguments.end()) {
				source.SendFailure(ChatComponent::Create()
					.Text("Missing arguments")
					.Color(ChatColor::Red)
					.Build());
				return 0;
			}

			int requestedLevel = 0;
			try {
				requestedLevel = std::stoi(levelIterator->second);
			} catch (const std::exception&) {
				source.SendFailure(ChatComponent::Create()
					.Text("Invalid level")
					.Color(ChatColor::Red)
					.Build());
				return 0;
			}
			if (requestedLevel < 1 || requestedLevel > 4) {
				source.SendFailure(ChatComponent::Create()
					.Text("Op level must be between 1 and 4")
					.Color(ChatColor::Red)
					.Build());
				return 0;
			}

			return ApplyOp(source, targetIterator->second, requestedLevel);
		});
		target->Then(levelArgument);

		root->Then(target);
		return root;
	}

}
