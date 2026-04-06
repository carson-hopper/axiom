#pragma once

#include "Axiom/Command/Command.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	using CommandCallback = std::function<void(CommandSender&, const std::vector<std::string>&)>;

	class CommandRegistry {
	public:
		void Register(Ref<Command> command);
		void Register(const std::string& name, const std::string& description, CommandCallback callback);
		bool Dispatch(CommandSender& sender, const std::string& input);
		std::vector<std::string> TabComplete(CommandSender& sender, const std::string& partial);
		void Unregister(const std::string& name);

	private:
		std::unordered_map<std::string, Ref<Command>> m_Commands;
	};

}
