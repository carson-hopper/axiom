#include "axpch.h"
#include "CommandSender.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Chat/MiniMessage.h"
#include "Axiom/Core/Log.h"

namespace Axiom {

	const std::string ConsoleSender::s_Name = "Console";

	void CommandSender::SendPlainMessage(const std::string& text) {
		const auto component = ChatComponent::Create()
			.Text(text)
			.Build();
		SendMessage(component);
	}

	void CommandSender::SendMiniMessage(const std::string& miniMessage) {
		const auto component = MiniMessage::Parse(miniMessage);
		SendMessage(component);
	}

	void ConsoleSender::SendMessage(const Ref<ChatComponent>& message) {
		// For console, just log the plain text
		// In production, you might want to strip color codes
		AX_CORE_INFO("[Chat] {}", message->GetPlainText());
	}

}
