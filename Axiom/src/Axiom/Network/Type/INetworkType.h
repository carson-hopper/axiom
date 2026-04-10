#pragma once

namespace Axiom::Net {

/**
 * Marker interface for protocol-aware field types.
 * Types deriving from this are auto-serialized via
 * their Read/Write methods instead of constexpr if.
 */
class INetworkType {
public:
	virtual ~INetworkType() = default;
};

}
