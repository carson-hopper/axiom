cmake_minimum_required(VERSION 3.28)
project(Axiom VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----- Options ---------------------------------------------------

option(AX_ENABLE_ASSERTS "Enable assertions in debug builds" ON)

# ----- Dependencies ----------------------------------------------

# spdlog (header-only mode)
add_subdirectory(vendor/spdlog EXCLUDE_FROM_ALL)

# yaml-cpp
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)

# nlohmann/json (header-only)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/json EXCLUDE_FROM_ALL)

# asio (header-only, standalone)
add_library(asio INTERFACE
		src/axpch.h
		src/Axiom/Network/Packet/26.1/Packets.h)
target_include_directories(asio INTERFACE vendor/asio/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

# OpenSSL
find_package(OpenSSL REQUIRED)

# zlib
find_package(ZLIB REQUIRED)

# ----- Axiom Server ----------------------------------------------

file(GLOB_RECURSE AXIOM_SOURCES
	"src/Axiom/*.h"
	"src/Axiom/*.cpp"
)

add_executable(axiom-server ${AXIOM_SOURCES}
        src/Axiom/Core/Timestep.h
		src/Axiom/Core/UUID.h
		src/Axiom/Core/UUID.cpp
		src/axpch.cpp
        src/Axiom/Core/EntryPoint.h
		src/Axiom/Core/Timer.h
		src/Axiom/Core/TickStack.cpp
		src/Axiom/Core/TickStack.h)

target_include_directories(axiom-server PRIVATE src)

target_link_libraries(axiom-server PRIVATE
	spdlog::spdlog_header_only
	yaml-cpp
	nlohmann_json::nlohmann_json
	asio
	OpenSSL::SSL
	OpenSSL::Crypto
	ZLIB::ZLIB
)

target_compile_definitions(axiom-server PRIVATE
	$<$<CONFIG:Debug>:AX_DEBUG>
	$<$<CONFIG:Debug>:AX_ENABLE_ASSERTS>
	$<$<CONFIG:Release>:AX_RELEASE>
)

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_compile_definitions(axiom-server PRIVATE AX_PLATFORM_LINUX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	target_compile_definitions(axiom-server PRIVATE AX_PLATFORM_MACOS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_compile_definitions(axiom-server PRIVATE AX_PLATFORM_WINDOWS)
endif()

# Warnings
if(MSVC)
	target_compile_options(axiom-server PRIVATE /W4)
else()
	target_compile_options(axiom-server PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy data directory next to executable after build
add_custom_command(TARGET axiom-server POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/data
		$<TARGET_FILE_DIR:axiom-server>/data
	COMMENT "Copying data directory to build output"
)

# ----- Tests -----------------------------------------------------

option(AX_BUILD_TESTS "Build unit tests" ON)

if(AX_BUILD_TESTS)
	file(GLOB_RECURSE TEST_SOURCES
		"tests/*.h"
		"tests/*.cpp"
	)

	add_executable(axiom-tests ${TEST_SOURCES})

	target_include_directories(axiom-tests PRIVATE
		src
		tests
	)

	target_link_libraries(axiom-tests PRIVATE
		spdlog::spdlog_header_only
		yaml-cpp
		nlohmann_json::nlohmann_json
		asio
		OpenSSL::SSL
		OpenSSL::Crypto
		ZLIB::ZLIB
	)

	target_compile_definitions(axiom-tests PRIVATE
		AX_DEBUG
		AX_ENABLE_ASSERTS
	)

	# Platform detection for tests
	if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		target_compile_definitions(axiom-tests PRIVATE AX_PLATFORM_LINUX)
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		target_compile_definitions(axiom-tests PRIVATE AX_PLATFORM_MACOS)
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		target_compile_definitions(axiom-tests PRIVATE AX_PLATFORM_WINDOWS)
	endif()

	# Warnings for tests
	if(MSVC)
		target_compile_options(axiom-tests PRIVATE /W4)
	else()
		target_compile_options(axiom-tests PRIVATE -Wall -Wextra -Wpedantic)
	endif()

	# Add test target
	add_custom_target(test
		COMMAND axiom-tests
		DEPENDS axiom-tests
		COMMENT "Running unit tests"
	)
endif()
