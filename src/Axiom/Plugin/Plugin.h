#pragma once

#include "Axiom/Core/Base.h"

#include <string>

namespace Axiom {

	class PluginContext;

	class Plugin {
	public:
		virtual ~Plugin() = default;

		virtual void OnEnable(PluginContext& context) = 0;
		virtual void OnDisable() = 0;

		const std::string& Name() const { return m_Name; }
		const std::string& Version() const { return m_Version; }

	protected:
		std::string m_Name;
		std::string m_Version;
	};

}
