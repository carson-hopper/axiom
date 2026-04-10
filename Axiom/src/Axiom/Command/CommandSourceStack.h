#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/Base.h"
#include "Axiom/Core/Math.h"
#include "Axiom/Environment/Entity/Player.h"

#include <string>

namespace Axiom {

/**
 * Execution context carried through the command
 * tree while dispatching. Wraps the source entity,
 * its permission level, world position, and convenience
 * methods for sending feedback.
 */
class CommandSourceStack {
public:
	CommandSourceStack(const std::string& name, int permissionLevel,
		const Vector3& position, Ref<Player> player);

	/**
	 * Console source with maximum permissions.
	 */
	static CommandSourceStack Console();

	const std::string& GetName() const { return m_Name; }
	int GetPermissionLevel() const { return m_PermissionLevel; }
	const Vector3& GetPosition() const { return m_Position; }
	Ref<Player> GetPlayer() const { return m_Player; }
	bool IsPlayer() const { return m_Player != nullptr; }

	/**
	 * Returns true when the source has the required
	 * operator level to run a command.
	 */
	bool HasPermission(int level) const;

	/**
	 * Returns true when the source has the named permission.
	 * Delegates to the underlying player's HasPermission; console
	 * is granted every permission.
	 */
	bool HasPermission(const std::string& permission) const;

	/**
	 * Sends a success-style message to the source.
	 */
	void SendMessage(const ChatText& component);

	/**
	 * Sends a failure-style message to the source.
	 */
	void SendFailure(const ChatText& component);

private:
	std::string m_Name;
	int m_PermissionLevel;
	Vector3 m_Position;
	Ref<Player> m_Player;
};

}
