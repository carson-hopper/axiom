#pragma once

#include "Axiom/Network/Type/INetworkType.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom::Net {

/**
 * Base template for protocol-aware field types.
 * Subclasses implement ReadImpl/WriteImpl to define
 * their wire format. The Packet CRTP base calls
 * Read/Write automatically during Parse/Write.
 */
template<typename T>
class NetworkType : public INetworkType {
public:
	NetworkType() = default;
	NetworkType(T value) : m_Value(std::move(value)) {}

	void Read(NetworkBuffer& buffer) {
		m_Value = ReadImpl(buffer);
	}

	void Write(NetworkBuffer& buffer) const {
		WriteImpl(buffer);
	}

	T& GetValue() { return m_Value; }
	const T& GetValue() const { return m_Value; }
	void SetValue(T value) { m_Value = std::move(value); }

	operator T&() { return m_Value; }
	operator const T&() const { return m_Value; }

	T* operator->() { return &m_Value; }
	const T* operator->() const { return &m_Value; }

protected:
	virtual T ReadImpl(NetworkBuffer& buffer) = 0;
	virtual void WriteImpl(NetworkBuffer& buffer) const = 0;

	T m_Value{};
};

}
