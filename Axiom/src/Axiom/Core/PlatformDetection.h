#pragma once

#ifdef _WIN32
	#ifdef _WIN64
		#define AX_PLATFORM_WINDOWS
	#else
		#error "x86 builds are not supported"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_OS_MAC
		#ifndef AX_PLATFORM_MACOS
			#define AX_PLATFORM_MACOS
		#endif
	#else
		#error "Unsupported Apple platform"
	#endif
#elif defined(__linux__)
	#define AX_PLATFORM_LINUX
#else
	#error "Unsupported platform"
#endif

#if defined(__GNUC__)
	#if defined(__clang__)
		#define AX_COMPILER_CLANG
	#else
		#define AX_COMPILER_GCC
	#endif
#elif defined(_MSC_VER)
	#define AX_COMPILER_MSVC
#endif

#ifdef AX_COMPILER_MSVC
	#define AX_FORCE_INLINE __forceinline
	#define AX_EXPLICIT_STATIC static
#elif defined(__GNUC__)
	#define AX_FORCE_INLINE __attribute__((always_inline)) inline
	#define AX_EXPLICIT_STATIC
#else
	#define AX_FORCE_INLINE inline
	#define AX_EXPLICIT_STATIC
#endif