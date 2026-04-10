#include "CommandRegistry.h"

#include "Axiom/Core/Log.h"

#include <algorithm>

namespace Axiom {

	void CommandRegistry::Register(Ref<Command> command) {
		auto tree = command->BuildTree();
		if (tree) {
			tree->Requires(command->RequiredPermissionLevel());
			if (!command->RequiredPermission().empty()) {
				tree->Requires(command->RequiredPermission());
			}
			m_Dispatcher.Register(tree);
		}
		m_Commands.push_back(std::move(command));
	}

	bool CommandRegistry::Dispatch(CommandSourceStack& source, const std::string& input) {
		return m_Dispatcher.Dispatch(source, input) != 0;
	}

	std::vector<std::string> CommandRegistry::TabComplete(CommandSourceStack& source,
		const std::string& partial) {
		return m_Dispatcher.GetSuggestions(source, partial);
	}

	void CommandRegistry::Unregister(const std::string& name) {
		std::erase_if(m_Commands, [&](const Ref<Command>& command) {
			return command->Name() == name;
		});
		// Note: CommandDispatcher doesn't expose a removal API; the
		// root node remains but we won't rebuild the dispatcher here.
	}

	bool CommandRegistry::HasCommand(const std::string& name) const {
		return std::any_of(m_Commands.begin(), m_Commands.end(),
			[&](const Ref<Command>& command) {
				return command->Name() == name;
			});
	}

	std::vector<std::string> CommandRegistry::GetCommandNames() const {
		std::vector<std::string> names;
		names.reserve(m_Commands.size());
		for (const auto& command : m_Commands) {
			names.push_back(command->Name());
		}
		return names;
	}

}
