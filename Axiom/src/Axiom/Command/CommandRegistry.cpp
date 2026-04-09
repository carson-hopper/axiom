#include "CommandRegistry.h"

#include "Axiom/Core/Log.h"

#include <sstream>

namespace Axiom {

	class LambdaCommand : public Command {
	public:
		LambdaCommand(std::string name, std::string description, CommandCallback callback)
			: m_Name(std::move(name))
			, m_Description(std::move(description))
			, m_Callback(std::move(callback)) {}

		const std::string& Name() const override { return m_Name; }
		const std::string& Description() const override { return m_Description; }

		void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override {
			m_Callback(sender, arguments);
		}

	private:
		std::string m_Name;
		std::string m_Description;
		CommandCallback m_Callback;
	};

	void CommandRegistry::Register(Ref<Command> command) {
		const std::string& name = command->Name();
		AX_CORE_TRACE("Registered command: {}", name);
		m_Commands[name] = std::move(command);
	}

	void CommandRegistry::Register(const std::string& name, const std::string& description, CommandCallback callback) {
		Register(CreateRef<LambdaCommand>(name, description, std::move(callback)));
	}

	bool CommandRegistry::Dispatch(CommandSender& sender, const std::string& input) {
		std::istringstream stream(input);
		std::string commandName;
		stream >> commandName;

		auto iterator = m_Commands.find(commandName);
		if (iterator == m_Commands.end()) {
			sender.SendPlainMessage("Unknown command: " + commandName);
			return false;
		}

		std::vector<std::string> arguments;
		std::string argument;
		while (stream >> argument) {
			arguments.push_back(std::move(argument));
		}

		iterator->second->Execute(sender, arguments);
		return true;
	}

	std::vector<std::string> CommandRegistry::TabComplete(CommandSender& sender, const std::string& partial) {
		std::vector<std::string> completions;

		std::istringstream stream(partial);
		std::string commandName;
		stream >> commandName;

		bool hasSpace = !partial.empty() && partial.back() == ' ';
		bool isCompletingCommand = !hasSpace && partial.find(' ') == std::string::npos;

		if (isCompletingCommand) {
			for (auto& [name, command] : m_Commands) {
				if (name.starts_with(commandName)) {
					completions.push_back(name);
				}
			}
		} else {
			auto iterator = m_Commands.find(commandName);
			if (iterator != m_Commands.end()) {
				std::vector<std::string> arguments;
				std::string argument;
				while (stream >> argument) {
					arguments.push_back(std::move(argument));
				}
				completions = iterator->second->TabComplete(sender, arguments);
			}
		}

		return completions;
	}

	void CommandRegistry::Unregister(const std::string& name) {
		m_Commands.erase(name);
	}

}
