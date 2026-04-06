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
add_library(asio INTERFACE)
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

add_executable(axiom-server ${AXIOM_SOURCES})

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
