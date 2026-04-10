#pragma once

#include "Axiom/Network/Type/INetworkType.h"
#include "Axiom/Network/NetworkBuffer.h"

namespace Axiom::Net {

/**
 * Base template for protocol-aware field types.
 * Subclasses implement ReadImpl/WriteImpl to define
 * their wire format. The Packet CRTP base calls
 * Read/Write automatically during Parse/Write.
 *
 * The default template stores a value of type T and
 * delegates to ReadImpl / WriteImpl. For polymorphic
 * hierarchies with no natural stored value (e.g. NBT
 * tags), use the NetworkType<void> specialization below.
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

/**
 * Polymorphic specialization for types that serialize themselves
 * without a wrapped value (e.g. NBT tag hierarchies, complex
 * containers). Subclasses override Read / Write directly.
 */
template<>
class NetworkType<void> : public INetworkType {
public:
	~NetworkType() override = default;

	virtual void Read(NetworkBuffer& buffer) = 0;
	virtual void Write(NetworkBuffer& buffer) const = 0;
};

}
