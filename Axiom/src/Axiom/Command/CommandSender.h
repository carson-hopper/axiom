#pragma once

#include "Axiom/Chat/ChatComponent.h"
#include "Axiom/Core/Base.h"

namespace Axiom {

class CommandSender : public virtual RefCounted {
public:
	~CommandSender() override = default;

	/**
	 * Send a chat message. Accepts a Ref<ChatComponent>, std::string,
	 * const char*, or nullptr via the implicit ChatText wrapper.
	 */
	virtual void SendMessage(const ChatText& message) = 0;

	/**
	 * Send a MiniMessage format message (convenience method).
	 * Supports tags like <red>, <bold>, <click:run_command:/help>, etc.
	 *
	 * @param miniMessage The MiniMessage format string.
	 */
	void SendMiniMessage(const std::string& miniMessage);

	virtual const std::string& Name() const = 0;
	virtual bool IsPlayer() const = 0;
	virtual bool HasPermission(const std::string& permission) const = 0;
};

class ConsoleSender : public CommandSender {
public:
	void SendMessage(const ChatText& message) override;
	const std::string& Name() const override { return s_Name; }
	bool IsPlayer() const override { return false; }
	bool HasPermission(const std::string& /*permission*/) const override { return true; }

private:
	static const std::string s_Name;
};

}
