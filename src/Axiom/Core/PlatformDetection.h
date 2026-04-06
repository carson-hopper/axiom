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
		#define AX_PLATFORM_MACOS
	#else
		#error "Unsupported Apple platform"
	#endif
#elif defined(__linux__)
	#define AX_PLATFORM_LINUX
#else
	#error "Unsupported platform"
#endif
