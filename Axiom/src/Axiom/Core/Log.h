#pragma once

#include "Axiom/Core/Base.h"

#if defined(_MSC_VER)
	#pragma warning(push, 0)
#endif

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

namespace Axiom {

	class Log {
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& CoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& PluginLogger() { return s_PluginLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_PluginLogger;
	};

}

// ----- Core logging macros --------------------------------------

#define AX_CORE_TRACE(...)    ::Axiom::Log::CoreLogger()->trace(__VA_ARGS__)
#define AX_CORE_INFO(...)     ::Axiom::Log::CoreLogger()->info(__VA_ARGS__)
#define AX_CORE_WARN(...)     ::Axiom::Log::CoreLogger()->warn(__VA_ARGS__)
#define AX_CORE_ERROR(...)    ::Axiom::Log::CoreLogger()->error(__VA_ARGS__)
#define AX_CORE_CRITICAL(...) ::Axiom::Log::CoreLogger()->critical(__VA_ARGS__)

// ----- Plugin logging macros ------------------------------------

#define AX_TRACE(...)         ::Axiom::Log::PluginLogger()->trace(__VA_ARGS__)
#define AX_INFO(...)          ::Axiom::Log::PluginLogger()->info(__VA_ARGS__)
#define AX_WARN(...)          ::Axiom::Log::PluginLogger()->warn(__VA_ARGS__)
#define AX_ERROR(...)         ::Axiom::Log::PluginLogger()->error(__VA_ARGS__)
#define AX_CRITICAL(...)      ::Axiom::Log::PluginLogger()->critical(__VA_ARGS__)
