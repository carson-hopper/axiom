#include "CommandSender.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	const std::string ConsoleSender::s_Name = "Console";

	void ConsoleSender::SendMessage(const std::string& message) {
		AX_CORE_INFO("{}", message);
	}

}
