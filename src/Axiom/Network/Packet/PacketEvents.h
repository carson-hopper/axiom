#pragma once

#include "Axiom/Event/Event.h"
#include "Axiom/Network/Protocol.h"

#include <cstdint>
#include <memory>

namespace Axiom {

	class Connection;
	class ServerboundPacket;

	/**
	 * Fired before a packet's Handle() is called.
	 * Set m_Handled = true to cancel the packet (skip Handle).
	 */
	class PacketReceiveEvent : public Event {
	public:
		PacketReceiveEvent(std::shared_ptr<Connection> connection, ConnectionState state,
			int32_t packetId, ServerboundPacket* packet)
			: m_Connection(std::move(connection))
			, m_State(state)
			, m_PacketId(packetId)
			, m_Packet(packet) {}

		std::shared_ptr<Connection>& GetConnection() { return m_Connection; }
		ConnectionState GetState() const { return m_State; }
		int32_t GetPacketId() const { return m_PacketId; }
		ServerboundPacket* GetPacket() { return m_Packet; }

		template<typename T>
		T* GetPacketAs() { return dynamic_cast<T*>(m_Packet); }

		AX_EVENT_CLASS_TYPE(PacketReceive)
		AX_EVENT_CLASS_CATEGORY(EventCategoryServer)

	private:
		std::shared_ptr<Connection> m_Connection;
		ConnectionState m_State;
		int32_t m_PacketId;
		ServerboundPacket* m_Packet;
	};

	/**
	 * Fired after a packet's Handle() has completed successfully.
	 * Not cancellable — the packet has already been processed.
	 */
	class PacketHandledEvent : public Event {
	public:
		PacketHandledEvent(std::shared_ptr<Connection> connection, ConnectionState state,
			int32_t packetId, ServerboundPacket* packet)
			: m_Connection(std::move(connection))
			, m_State(state)
			, m_PacketId(packetId)
			, m_Packet(packet) {}

		std::shared_ptr<Connection>& GetConnection() { return m_Connection; }
		ConnectionState GetState() const { return m_State; }
		int32_t GetPacketId() const { return m_PacketId; }
		ServerboundPacket* GetPacket() { return m_Packet; }

		template<typename T>
		T* GetPacketAs() { return dynamic_cast<T*>(m_Packet); }

		AX_EVENT_CLASS_TYPE(PacketHandled)
		AX_EVENT_CLASS_CATEGORY(EventCategoryServer)

	private:
		std::shared_ptr<Connection> m_Connection;
		ConnectionState m_State;
		int32_t m_PacketId;
		ServerboundPacket* m_Packet;
	};

}
