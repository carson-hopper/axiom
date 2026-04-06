#pragma once

#include "Axiom/Core/Log.h"

#ifdef AX_ENABLE_ASSERTS
	#define AX_INTERNAL_ASSERT_IMPL(type, check, message, ...) \
		do { \
			if (!(check)) { \
				AX##type##ERROR(message, ##__VA_ARGS__); \
				AX_DEBUGBREAK(); \
			} \
		} while (false)

	#define AX_INTERNAL_ASSERT_WITH_MSG(type, check, ...) AX_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{}' failed: {}", AX_STRINGIFY_MACRO(check), __VA_ARGS__)
	#define AX_INTERNAL_ASSERT_NO_MSG(type, check) AX_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{}' failed at {}:{}", AX_STRINGIFY_MACRO(check), __FILE__, __LINE__)

	#define AX_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define AX_INTERNAL_ASSERT_GET_MACRO(...) AX_EXPAND_MACRO(AX_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, AX_INTERNAL_ASSERT_WITH_MSG, AX_INTERNAL_ASSERT_NO_MSG))

	#define AX_ASSERT(...)      AX_EXPAND_MACRO(AX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
	#define AX_CORE_ASSERT(...) AX_EXPAND_MACRO(AX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
	#define AX_ASSERT(...)      ((void)0)
	#define AX_CORE_ASSERT(...) ((void)0)
#endif
