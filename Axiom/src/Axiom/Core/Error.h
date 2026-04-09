#pragma once

#include <expected>
#include <string>
#include <system_error>

namespace Axiom {

/**
 * Error codes for Axiom operations.
 *
 * These error codes are used with std::expected to communicate
 * failures without exceptions.
 */
enum class ErrorCode {
	Success = 0,

	// Network errors
	ConnectionClosed,
	ConnectionReset,
	ConnectionTimeout,
	InvalidPacket,
	DecompressionFailed,
	CompressionFailed,
	EncryptionFailed,
	DecryptionFailed,

	// World/Chunk errors
	ChunkNotLoaded,
	BlockOutOfBounds,
	InvalidBlockState,

	// Player errors
	PlayerNotFound,
	InvalidGameMode,
	InventoryFull,

	// Configuration errors
	ConfigNotFound,
	ConfigParseError,
	ConfigInvalidValue,

	// Plugin errors
	PluginNotFound,
	PluginAlreadyLoaded,
	PluginLoadFailed,
	PluginDependencyMissing,

	// Command errors
	CommandNotFound,
	CommandInvalidArguments,
	CommandNoPermission,

	// General errors
	NotImplemented,
	InvalidArgument,
	InternalError,
	ResourceExhausted,
};

/**
 * Error category for Axiom error codes.
 */
class AxiomErrorCategory : public std::error_category {
public:
	const char* name() const noexcept override {
		return "axiom";
	}

	std::string message(int ev) const override {
		switch (static_cast<ErrorCode>(ev)) {
			case ErrorCode::Success: return "Success";
			case ErrorCode::ConnectionClosed: return "Connection closed";
			case ErrorCode::ConnectionReset: return "Connection reset";
			case ErrorCode::ConnectionTimeout: return "Connection timeout";
			case ErrorCode::InvalidPacket: return "Invalid packet format";
			case ErrorCode::DecompressionFailed: return "Packet decompression failed";
			case ErrorCode::CompressionFailed: return "Packet compression failed";
			case ErrorCode::EncryptionFailed: return "Encryption failed";
			case ErrorCode::DecryptionFailed: return "Decryption failed";
			case ErrorCode::ChunkNotLoaded: return "Chunk not loaded";
			case ErrorCode::BlockOutOfBounds: return "Block position out of bounds";
			case ErrorCode::InvalidBlockState: return "Invalid block state ID";
			case ErrorCode::PlayerNotFound: return "Player not found";
			case ErrorCode::InvalidGameMode: return "Invalid game mode";
			case ErrorCode::InventoryFull: return "Inventory is full";
			case ErrorCode::ConfigNotFound: return "Configuration file not found";
			case ErrorCode::ConfigParseError: return "Configuration parse error";
			case ErrorCode::ConfigInvalidValue: return "Invalid configuration value";
			case ErrorCode::PluginNotFound: return "Plugin not found";
			case ErrorCode::PluginAlreadyLoaded: return "Plugin already loaded";
			case ErrorCode::PluginLoadFailed: return "Failed to load plugin";
			case ErrorCode::PluginDependencyMissing: return "Plugin dependency missing";
			case ErrorCode::CommandNotFound: return "Command not found";
			case ErrorCode::CommandInvalidArguments: return "Invalid command arguments";
			case ErrorCode::CommandNoPermission: return "No permission for command";
			case ErrorCode::NotImplemented: return "Feature not implemented";
			case ErrorCode::InvalidArgument: return "Invalid argument";
			case ErrorCode::InternalError: return "Internal error";
			case ErrorCode::ResourceExhausted: return "Resource exhausted";
			default: return "Unknown error";
		}
	}
};

inline const std::error_category& GetAxiomErrorCategory() {
	static class AxiomErrorCategory s_Category;
	return s_Category;
}

inline std::error_code make_error_code(ErrorCode e) {
	return {static_cast<int>(e), GetAxiomErrorCategory()};
}

/**
 * Helper type alias for std::expected with Axiom error codes.
 */
template<typename T>
using Result = std::expected<T, std::error_code>;

/**
 * Helper function to create an error result.
 */
template<typename T = void>
inline Result<T> MakeError(ErrorCode code) {
	return std::unexpected(make_error_code(code));
}

} // namespace Axiom

// Enable std::error_code conversion for ErrorCode
namespace std {
	template<>
	struct is_error_code_enum<Axiom::ErrorCode> : true_type {};
}