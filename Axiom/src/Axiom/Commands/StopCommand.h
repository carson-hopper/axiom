#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Stops the server gracefully.
 *
 * Usage: /stop
 * Permission: server.stop
 */
class StopCommand : public Command {
public:
	const std::string& Name() const override {
		static std::string name = "stop";
		return name;
	}

	const std::string& Description() const override {
		static std::string desc = "Stop the server";
		return desc;
	}

	const std::string& Usage() const {
		static std::string usage = "/stop";
		return usage;
	}

	const std::string& Permission() const {
		static std::string perm = "server.stop";
		return perm;
	}

	void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

	std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;
};

}
