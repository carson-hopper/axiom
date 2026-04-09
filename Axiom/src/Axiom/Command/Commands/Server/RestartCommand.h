#pragma once

#include "Axiom/Command/Command.h"

namespace Axiom {

/**
 * Saves all chunks and restarts the server process.
 *
 * Usage: /restart
 * Permission: server.restart
 */
class RestartCommand : public Command {
public:
	const std::string& Name() const override {
		static std::string name = "restart";
		return name;
	}

	const std::string& Description() const override {
		static std::string desc = "Save all chunks and restart the server";
		return desc;
	}
    
    const std::string& Usage() const {
        static std::string usage = "/restart";
        return usage;
    }
    
    const std::string& Permission() const {
        static std::string perm = "axiom.server.restart";
        return perm;
    }

	void Execute(CommandSender& sender, const std::vector<std::string>& arguments) override;

	std::vector<std::string> TabComplete(CommandSender& sender, const std::vector<std::string>& arguments) override;
};

}
