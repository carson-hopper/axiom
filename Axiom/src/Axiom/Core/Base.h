#pragma once

#include "Axiom/Core/PlatformDetection.h"

#ifdef AX_DEBUG
	#if defined(AX_PLATFORM_WINDOWS)
		#define AX_DEBUGBREAK() __debugbreak()
	#elif defined(AX_PLATFORM_LINUX) || defined(AX_PLATFORM_MACOS)
		#include <csignal>
		#define AX_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "AX_DEBUGBREAK not supported on this platform"
	#endif
	#ifndef AX_ENABLE_ASSERTS
		#define AX_ENABLE_ASSERTS
	#endif
#else
	#define AX_DEBUGBREAK()
#endif

#define AX_EXPAND_MACRO(x) x
#define AX_STRINGIFY_IMPL(x) #x
#define AX_STRINGIFY_MACRO(x) AX_STRINGIFY_IMPL(x)

#define BIT(x) (1 << (x))

#include "Axiom/Utilities/Memory/Ref.h"

#include <atomic>
#include <memory>
#include <utility>

namespace Axiom {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename... Args>
	[[nodiscard]] constexpr Scope<T> CreateScope(Args&&... arguments) {
		return std::make_unique<T>(std::forward<Args>(arguments)...);
	}

}