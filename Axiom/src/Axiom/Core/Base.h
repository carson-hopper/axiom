#pragma once

#include "Axiom/Core/PlatformDetection.h"

#include <atomic>
#include <memory>
#include <utility>

// ----- Debug break ----------------------------------------------

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

// ----- Macro helpers --------------------------------------------

#define AX_EXPAND_MACRO(x) x
#define AX_STRINGIFY_IMPL(x) #x
#define AX_STRINGIFY_MACRO(x) AX_STRINGIFY_IMPL(x)

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

	/**
	 * Monotonic identifier for objects that should not
	 * be keyed by raw pointer (e.g. connections).
	 */
	template<typename Tag>
	class TypedId {
	public:
		TypedId() : m_Value(s_Next++) {}
		explicit TypedId(uint64_t value) : m_Value(value) {}

		uint64_t Value() const { return m_Value; }

		bool operator==(const TypedId&) const = default;
		auto operator<=>(const TypedId&) const = default;

	private:
		uint64_t m_Value;
		static inline std::atomic<uint64_t> s_Next{1};
	};

	struct ConnectionIdTag {};
	using ConnectionId = TypedId<ConnectionIdTag>;

}

namespace std {

	template<typename Tag>
	struct hash<Axiom::TypedId<Tag>> {
		std::size_t operator()(const Axiom::TypedId<Tag>& identifier) const noexcept {
			return std::hash<uint64_t>{}(identifier.Value());
		}
	};

}
