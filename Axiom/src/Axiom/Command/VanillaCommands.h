#pragma once

namespace Axiom {

class CommandDispatcher;

/**
 * Registers all built-in vanilla commands on the
 * given dispatcher. Each function adds a command tree
 * with appropriate argument nodes and execute callbacks.
 */
namespace VanillaCommands {

	void RegisterAll(CommandDispatcher& dispatcher);

	void RegisterHelp(CommandDispatcher& dispatcher);
	void RegisterList(CommandDispatcher& dispatcher);
	void RegisterStop(CommandDispatcher& dispatcher);
	void RegisterGamemode(CommandDispatcher& dispatcher);
	void RegisterDifficulty(CommandDispatcher& dispatcher);
	void RegisterTime(CommandDispatcher& dispatcher);
	void RegisterTeleport(CommandDispatcher& dispatcher);
	void RegisterSetWorldSpawn(CommandDispatcher& dispatcher);
	void RegisterGamerule(CommandDispatcher& dispatcher);

}

}
