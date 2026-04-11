#include "axpch.h"
#include "Axiom/Command/Commands/Server/DeopCommand.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/Parsers.h"
#include "Axiom/Config/AdminFileStore.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Environment/Entity/Player.h"
#include "Axiom/Network/Packet/Play/Clientbound/Commands.h"

namespace Axiom {

	Ref<LiteralNode> DeopCommand::BuildTree() {
		auto root = Literal("deop");
		root->Requires(RequiredPermissionLevel());

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

			const std::string& targetName = iterator->second;
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

			if (!adminFiles.IsOp(target->GetUuid().ToString())) {
				source.SendFailure(ChatComponent::Create()
					.Text(targetName + " is not an operator")
					.Color(ChatColor::Red)
					.Build());
				return 0;
			}

			// Prevent privilege escalation: players
			// can only deop targets whose level is
			// strictly below their own. Console has
			// no such restriction.
			if (source.IsPlayer()) {
				const int sourceLevel = source.GetPermissionLevel();
				const int targetLevel = static_cast<int>(target->GetOpLevel());
				if (targetLevel >= sourceLevel) {
					source.SendFailure(ChatComponent::Create()
						.Text("You cannot deop " + targetName
							+ " because their op level is not below yours")
						.Color(ChatColor::Red)
						.Build());
					return 0;
				}
			}

			adminFiles.RemoveOp(target->GetUuid().ToString());
			target->SetOpLevel(OpLevel::None);

			// Resend the command graph so the
			// target immediately loses access
			// to any now-restricted commands.
			auto refreshPacket = Ref<Play::Clientbound::CommandsPacket>::Create(
				application.Commands().GetRootNodes(), target);
			NetworkBuffer payload;
			refreshPacket->Write(payload);
			target->GetConnection()->SendRawPacket(
				refreshPacket->GetPacketId(), payload);

			source.SendMessage(ChatComponent::Create()
				.Text("Removed " + targetName + " from operators")
				.Color(ChatColor::Green)
				.Build());

			target->SendMessage(ChatComponent::Create()
				.Text("You are no longer a server operator")
				.Color(ChatColor::Yellow)
				.Build());

			return 1;
		});

		root->Then(target);
		return root;
	}

}
