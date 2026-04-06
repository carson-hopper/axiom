#pragma once

#include "Axiom/Core/PlatformDetection.h"

#include <memory>
#include <utility>

// ----- Debug break ----------------------------------------------

#ifdef AX_DEBUG
	#if defined(AX_PLATFORM_WINDOWS)
		#define AX_DEBUGBREAK() __debugbreak()
	#elif defined(AX_PLATFORM_LINUX) || defined(AX_PLATFORM_MACOS)
		#include <signal.h>
		#define AX_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "AX_DEBUGBREAK not supported on this platform"
	#endif
	#define AX_ENABLE_ASSERTS
#else
	#define AX_DEBUGBREAK()
#endif

// ----- Macro helpers --------------------------------------------

#define AX_EXPAND_MACRO(x) x
#define AX_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << (x))

#define AX_BIND_FN(function) [this](auto&&... arguments) { return this->function(std::forward<decltype(arguments)>(arguments)...); }

// ----- Smart pointer aliases ------------------------------------

namespace Axiom {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... arguments) {
		return std::make_unique<T>(std::forward<Args>(arguments)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... arguments) {
		return std::make_shared<T>(std::forward<Args>(arguments)...);
	}

}
