#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Command/CommandSender.h"

#include <string>
#include <vector>

namespace Axiom {

	class Command {
	public:
		virtual ~Command() = default;

		virtual const std::string& Name() const = 0;
		virtual const std::string& Description() const = 0;
		virtual void Execute(CommandSender& sender, const std::vector<std::string>& arguments) = 0;

		virtual std::vector<std::string> TabComplete(
			CommandSender& /*sender*/, const std::vector<std::string>& /*arguments*/) {

			return {};
		}
	};

}
