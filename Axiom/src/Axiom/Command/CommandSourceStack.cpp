#include "Axiom/Command/CommandSourceStack.h"

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Environment/Entity/Player.h"
#include "Axiom/Core/Log.h"

namespace Axiom {

CommandSourceStack::CommandSourceStack(const std::string& name,
	int permissionLevel, const Vector3& position, Ref<Player> player)
	: m_Name(name)
	, m_PermissionLevel(permissionLevel)
	, m_Position(position)
	, m_Player(std::move(player)) {}

CommandSourceStack CommandSourceStack::Console() {
	return CommandSourceStack("Server", 4, Vector3{}, nullptr);
}

bool CommandSourceStack::HasPermission(int level) const {
	return m_PermissionLevel >= level;
}

void CommandSourceStack::SendMessage(const Ref<ChatComponent>& component) {
	if (m_Player) {
		m_Player->SendMessage(component);
	} else {
		AX_CORE_INFO("{}", component->GetPlainText());
	}
}

void CommandSourceStack::SendFailure(const Ref<ChatComponent>& component) {
	if (m_Player) {
		m_Player->SendMessage(component);
	} else {
		AX_CORE_ERROR("{}", component->GetPlainText());
	}
}

}
