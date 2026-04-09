#include "Axiom/Command/VanillaCommands.h"

#include "Axiom/Command/CommandDispatcher.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Config/GameRules.h"
#include "Axiom/Core/Application.h"
#include "Axiom/Core/Log.h"

#include <sstream>

namespace Axiom::VanillaCommands {

void RegisterAll(CommandDispatcher& dispatcher) {
	RegisterHelp(dispatcher);
	RegisterList(dispatcher);
	RegisterStop(dispatcher);
	RegisterGamemode(dispatcher);
	RegisterDifficulty(dispatcher);
	RegisterTime(dispatcher);
	RegisterTeleport(dispatcher);
	RegisterSetWorldSpawn(dispatcher);
	RegisterGamerule(dispatcher);
}

// ----------------------------------------------------------------
// help
// ----------------------------------------------------------------

void RegisterHelp(CommandDispatcher& dispatcher) {
	auto helpNode = Literal("help");
	helpNode->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& /*arguments*/) -> int {
		(void)Application::Instance();
		source.SendMessage(ChatComponent::Create()
			.Text("--- Help ---")
			.Color(ChatColor::Gold)
			.Build());
		// List all registered commands from the old registry
		// as well as the dispatcher root literals
		source.SendMessage(ChatComponent::Create()
			.Text("Use /help <command> for more information")
			.Color(ChatColor::Gray)
			.Build());
		return 1;
	});
	dispatcher.Register(helpNode);
}

// ----------------------------------------------------------------
// list
// ----------------------------------------------------------------

void RegisterList(CommandDispatcher& dispatcher) {
	auto listNode = Literal("list");
	listNode->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& /*arguments*/) -> int {
		source.SendMessage(ChatComponent::Create()
			.Text("There are 0 players online")
			.Color(ChatColor::White)
			.Build());
		return 1;
	});
	dispatcher.Register(listNode);
}

// ----------------------------------------------------------------
// stop
// ----------------------------------------------------------------

void RegisterStop(CommandDispatcher& dispatcher) {
	auto stopNode = Literal("stop");
	stopNode->Requires(4);
	stopNode->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& /*arguments*/) -> int {
		source.SendMessage(ChatComponent::Create()
			.Text("Stopping the server...")
			.Color(ChatColor::Yellow)
			.Build());
		Application::Instance().Stop();
		return 1;
	});
	dispatcher.Register(stopNode);
}

// ----------------------------------------------------------------
// gamemode
// ----------------------------------------------------------------

void RegisterGamemode(CommandDispatcher& dispatcher) {
	auto gamemodeNode = Literal("gamemode");
	gamemodeNode->Requires(2);

	auto modeArg = Argument("mode", ArgumentType::GameMode);
	modeArg->WithSuggestions([](CommandSourceStack& /*source*/,
		const std::string& partial) -> std::vector<std::string> {
		std::vector<std::string> modes = {
			"survival", "creative", "adventure", "spectator"};
		std::vector<std::string> results;
		for (auto& mode : modes) {
			if (mode.find(partial) == 0) {
				results.push_back(mode);
			}
		}
		return results;
	});

	modeArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto iterator = arguments.find("mode");
		if (iterator == arguments.end()) {
			return 0;
		}
		source.SendMessage(ChatComponent::Create()
			.Text("Set game mode to " + iterator->second)
			.Color(ChatColor::Green)
			.Build());
		return 1;
	});

	gamemodeNode->Then(modeArg);
	dispatcher.Register(gamemodeNode);
}

// ----------------------------------------------------------------
// difficulty
// ----------------------------------------------------------------

void RegisterDifficulty(CommandDispatcher& dispatcher) {
	auto difficultyNode = Literal("difficulty");
	difficultyNode->Requires(2);

	auto levelArg = Argument("level", ArgumentType::String);
	levelArg->WithSuggestions([](CommandSourceStack& /*source*/,
		const std::string& partial) -> std::vector<std::string> {
		std::vector<std::string> levels = {
			"peaceful", "easy", "normal", "hard"};
		std::vector<std::string> results;
		for (auto& level : levels) {
			if (level.find(partial) == 0) {
				results.push_back(level);
			}
		}
		return results;
	});

	levelArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto iterator = arguments.find("level");
		if (iterator == arguments.end()) {
			return 0;
		}
		source.SendMessage(ChatComponent::Create()
			.Text("Set difficulty to " + iterator->second)
			.Color(ChatColor::Green)
			.Build());
		return 1;
	});

	difficultyNode->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& /*arguments*/) -> int {
		auto& config = Application::Instance().Config();
		source.SendMessage(ChatComponent::Create()
			.Text("Current difficulty: " + config.Difficulty())
			.Color(ChatColor::White)
			.Build());
		return 1;
	});

	difficultyNode->Then(levelArg);
	dispatcher.Register(difficultyNode);
}

// ----------------------------------------------------------------
// time
// ----------------------------------------------------------------

void RegisterTime(CommandDispatcher& dispatcher) {
	auto timeNode = Literal("time");
	timeNode->Requires(2);

	// time set <value>
	auto setNode = Literal("set");
	auto setValueArg = Argument("value", ArgumentType::Integer);
	setValueArg->WithSuggestions([](CommandSourceStack& /*source*/,
		const std::string& partial) -> std::vector<std::string> {
		std::vector<std::string> presets = {
			"day", "noon", "night", "midnight"};
		std::vector<std::string> results;
		for (auto& preset : presets) {
			if (preset.find(partial) == 0) {
				results.push_back(preset);
			}
		}
		return results;
	});
	setValueArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto iterator = arguments.find("value");
		if (iterator == arguments.end()) {
			return 0;
		}
		source.SendMessage(ChatComponent::Create()
			.Text("Set the time to " + iterator->second)
			.Color(ChatColor::Green)
			.Build());
		return 1;
	});
	setNode->Then(setValueArg);

	// time add <value>
	auto addNode = Literal("add");
	auto addValueArg = Argument("value", ArgumentType::Integer);
	addValueArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto iterator = arguments.find("value");
		if (iterator == arguments.end()) {
			return 0;
		}
		source.SendMessage(ChatComponent::Create()
			.Text("Added " + iterator->second + " to the time")
			.Color(ChatColor::Green)
			.Build());
		return 1;
	});
	addNode->Then(addValueArg);

	// time query <type>
	auto queryNode = Literal("query");
	auto queryTypeArg = Argument("type", ArgumentType::String);
	queryTypeArg->WithSuggestions([](CommandSourceStack& /*source*/,
		const std::string& partial) -> std::vector<std::string> {
		std::vector<std::string> types = {"daytime", "gametime", "day"};
		std::vector<std::string> results;
		for (auto& type : types) {
			if (type.find(partial) == 0) {
				results.push_back(type);
			}
		}
		return results;
	});
	queryTypeArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto iterator = arguments.find("type");
		if (iterator == arguments.end()) {
			return 0;
		}
		source.SendMessage(ChatComponent::Create()
			.Text("The " + iterator->second + " is 0")
			.Color(ChatColor::White)
			.Build());
		return 1;
	});
	queryNode->Then(queryTypeArg);

	timeNode->Then(setNode);
	timeNode->Then(addNode);
	timeNode->Then(queryNode);
	dispatcher.Register(timeNode);
}

// ----------------------------------------------------------------
// teleport
// ----------------------------------------------------------------

void RegisterTeleport(CommandDispatcher& dispatcher) {
	auto teleportNode = Literal("teleport");
	teleportNode->Requires(2);

	// teleport <target>
	auto targetArg = Argument("target", ArgumentType::Entity);
	targetArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto iterator = arguments.find("target");
		if (iterator == arguments.end()) {
			return 0;
		}
		source.SendMessage(ChatComponent::Create()
			.Text("Teleported to " + iterator->second)
			.Color(ChatColor::Green)
			.Build());
		return 1;
	});

	// teleport <target> <x> <y> <z>
	auto xArg = Argument("x", ArgumentType::Float);
	auto yArg = Argument("y", ArgumentType::Float);
	auto zArg = Argument("z", ArgumentType::Float);
	zArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto targetIterator = arguments.find("target");
		auto xIterator = arguments.find("x");
		auto yIterator = arguments.find("y");
		auto zIterator = arguments.find("z");
		if (targetIterator == arguments.end() ||
			xIterator == arguments.end() ||
			yIterator == arguments.end() ||
			zIterator == arguments.end()) {
			return 0;
		}
		source.SendMessage(ChatComponent::Create()
			.Text("Teleported " + targetIterator->second +
				" to " + xIterator->second +
				" " + yIterator->second +
				" " + zIterator->second)
			.Color(ChatColor::Green)
			.Build());
		return 1;
	});

	yArg->Then(zArg);
	xArg->Then(yArg);
	targetArg->Then(xArg);

	teleportNode->Then(targetArg);
	dispatcher.Register(teleportNode);
}

// ----------------------------------------------------------------
// setworldspawn
// ----------------------------------------------------------------

void RegisterSetWorldSpawn(CommandDispatcher& dispatcher) {
	auto spawnNode = Literal("setworldspawn");
	spawnNode->Requires(2);

	spawnNode->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& /*arguments*/) -> int {
		source.SendMessage(ChatComponent::Create()
			.Text("Set world spawn to current position")
			.Color(ChatColor::Green)
			.Build());
		return 1;
	});

	auto xArg = Argument("x", ArgumentType::Integer);
	auto yArg = Argument("y", ArgumentType::Integer);
	auto zArg = Argument("z", ArgumentType::Integer);
	zArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto xIterator = arguments.find("x");
		auto yIterator = arguments.find("y");
		auto zIterator = arguments.find("z");
		if (xIterator == arguments.end() ||
			yIterator == arguments.end() ||
			zIterator == arguments.end()) {
			return 0;
		}
		source.SendMessage(ChatComponent::Create()
			.Text("Set world spawn to " +
				xIterator->second + " " +
				yIterator->second + " " +
				zIterator->second)
			.Color(ChatColor::Green)
			.Build());
		return 1;
	});

	yArg->Then(zArg);
	xArg->Then(yArg);
	spawnNode->Then(xArg);
	dispatcher.Register(spawnNode);
}

// ----------------------------------------------------------------
// gamerule
// ----------------------------------------------------------------

void RegisterGamerule(CommandDispatcher& dispatcher) {
	auto gameruleNode = Literal("gamerule");
	gameruleNode->Requires(2);

	auto ruleArg = Argument("rule", ArgumentType::String);
	ruleArg->WithSuggestions([](CommandSourceStack& /*source*/,
		const std::string& partial) -> std::vector<std::string> {
		auto& rules = Application::Instance().Rules();
		auto names = rules.RuleNames();
		std::vector<std::string> results;
		for (auto& name : names) {
			if (name.find(partial) == 0) {
				results.push_back(name);
			}
		}
		return results;
	});

	// gamerule <rule> — query
	ruleArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto iterator = arguments.find("rule");
		if (iterator == arguments.end()) {
			return 0;
		}
		auto& rules = Application::Instance().Rules();
		if (!rules.Has(iterator->second)) {
			source.SendFailure(ChatComponent::Create()
				.Text("Unknown game rule: " + iterator->second)
				.Color(ChatColor::Red)
				.Build());
			return 0;
		}
		auto value = rules.Get(iterator->second);
		source.SendMessage(ChatComponent::Create()
			.Text(iterator->second + " = " + value.ToString())
			.Color(ChatColor::White)
			.Build());
		return 1;
	});

	// gamerule <rule> <value> — set
	auto valueArg = Argument("value", ArgumentType::String);
	valueArg->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& arguments) -> int {
		auto ruleIterator = arguments.find("rule");
		auto valueIterator = arguments.find("value");
		if (ruleIterator == arguments.end() ||
			valueIterator == arguments.end()) {
			return 0;
		}
		auto& rules = Application::Instance().Rules();
		if (!rules.SetFromString(ruleIterator->second, valueIterator->second)) {
			source.SendFailure(ChatComponent::Create()
				.Text("Failed to set " + ruleIterator->second +
					" to " + valueIterator->second)
				.Color(ChatColor::Red)
				.Build());
			return 0;
		}
		source.SendMessage(ChatComponent::Create()
			.Text("Set " + ruleIterator->second +
				" to " + valueIterator->second)
			.Color(ChatColor::Green)
			.Build());
		return 1;
	});

	ruleArg->Then(valueArg);
	gameruleNode->Then(ruleArg);

	// gamerule with no arguments lists all rules
	gameruleNode->Executes([](CommandSourceStack& source,
		const std::unordered_map<std::string, std::string>& /*arguments*/) -> int {
		auto& rules = Application::Instance().Rules();
		auto names = rules.RuleNames();
		source.SendMessage(ChatComponent::Create()
			.Text("--- Game Rules ---")
			.Color(ChatColor::Gold)
			.Build());
		for (auto& name : names) {
			auto value = rules.Get(name);
			source.SendMessage(ChatComponent::Create()
				.Text(name + " = " + value.ToString())
				.Color(ChatColor::Gray)
				.Build());
		}
		return 1;
	});

	dispatcher.Register(gameruleNode);
}

}
