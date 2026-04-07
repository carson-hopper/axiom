#pragma once

#include "Axiom/Core/Base.h"

namespace Axiom {

class ChatComponent;

class CommandSender {
public:
	virtual ~CommandSender() = default;

	/**
	 * Send a rich chat message using ChatComponent.
	 */
	virtual void SendMessage(const Ref<ChatComponent>& message) = 0;

	/**
	 * Send a plain text message (convenience method).
	 */
	void SendPlainMessage(const std::string& text);

	virtual const std::string& Name() const = 0;
	virtual bool IsPlayer() const = 0;
	virtual bool HasPermission(const std::string& permission) const = 0;
};

class ConsoleSender : public CommandSender {
public:
	void SendMessage(const Ref<ChatComponent>& message) override;
	const std::string& Name() const override { return s_Name; }
	bool IsPlayer() const override { return false; }
	bool HasPermission(const std::string& /*permission*/) const override { return true; }

private:
	static const std::string s_Name;
};

}
