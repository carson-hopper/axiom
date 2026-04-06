#pragma once

#include "Axiom/Core/Base.h"

#include <string>

namespace Axiom {

	class CommandSender {
	public:
		virtual ~CommandSender() = default;

		virtual void SendMessage(const std::string& message) = 0;
		virtual const std::string& Name() const = 0;
		virtual bool IsPlayer() const = 0;
		virtual bool HasPermission(const std::string& permission) const = 0;
	};

	class ConsoleSender : public CommandSender {
	public:
		void SendMessage(const std::string& message) override;
		const std::string& Name() const override { return s_Name; }
		bool IsPlayer() const override { return false; }
		bool HasPermission(const std::string& permission) const override { return true; }

	private:
		static const std::string s_Name;
	};

}
