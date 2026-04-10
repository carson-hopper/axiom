#include "axpch.h"
#include "CommandSender.h"

#include "Axiom/Chat/MiniMessage.h"
#include "Axiom/Core/Log.h"

namespace Axiom {

	const std::string ConsoleSender::s_Name = "Console";

	void CommandSender::SendMiniMessage(const std::string& miniMessage) {
		SendMessage(MiniMessage::Parse(miniMessage));
	}

	void ConsoleSender::SendMessage(const ChatText& message) {
		if (!message) {
			return;
		}
		AX_CORE_INFO("[Chat] {}", message.Component()->GetPlainText());
	}

}
