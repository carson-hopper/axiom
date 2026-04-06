#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Log.h"
#include "Axiom/Network/Protocol.h"
#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Packet/PacketContext.h"
#include "Axiom/Network/Packet/PacketEvents.h"
#include "Axiom/Network/Connection.h"
#include "Axiom/Event/EventBus.h"

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace Axiom {

	/**
	 * Version-templated packet registry.
	 *
	 * Maps (ConnectionState, packetId) to factory functions. On Dispatch:
	 *   1. Creates and decodes the packet
	 *   2. Publishes PacketReceiveEvent (cancellable by plugins)
	 *   3. Calls Handle() if not cancelled
	 *   4. Publishes PacketHandledEvent
	 *
	 * When a new MC version ships, specialize only the changed packets
	 * and create a PacketRegistry for the new version number.
	 */
	template<int32_t Version = PROTOCOL_VERSION>
	class PacketRegistry {
	public:
		using Factory = std::function<Scope<ServerboundPacket>()>;

		template<typename T>
		void Register() {
			auto key = MakeKey(T::PacketState, T::PacketId);
			m_Factories[key] = []() -> Scope<ServerboundPacket> {
				return CreateScope<T>();
			};
		}

		Scope<ServerboundPacket> Create(ConnectionState state, int32_t packetId) const {
			auto key = MakeKey(state, packetId);
			auto iterator = m_Factories.find(key);
			if (iterator == m_Factories.end()) {
				return nullptr;
			}
			return iterator->second();
		}

		bool Has(ConnectionState state, int32_t packetId) const {
			return m_Factories.contains(MakeKey(state, packetId));
		}

		/**
		 * Create, decode, fire pre-event, handle, fire post-event.
		 * Returns false if no packet registered for this state/id.
		 */
		bool Dispatch(ConnectionState state, int32_t packetId, NetworkBuffer& buffer,
			Ref<Connection> connection, PacketContext& context) const {

			auto packet = Create(state, packetId);
			if (!packet) {
				AX_CORE_WARN("Unhandled packet 0x{:02X} in state {}", packetId,
					static_cast<int>(state));
				return false;
			}

			packet->Decode(buffer);

			// Pre-event: plugins can cancel by setting m_Handled = true
			PacketReceiveEvent receiveEvent(connection, state, packetId, packet.get());
			context.Events().Publish(receiveEvent);

			if (receiveEvent.m_Handled) {
				return true;  // Packet was cancelled by a plugin
			}

			packet->Handle(connection, context);

			// Post-event: informational, not cancellable
			PacketHandledEvent handledEvent(connection, state, packetId, packet.get());
			context.Events().Publish(handledEvent);

			return true;
		}

	private:
		static uint64_t MakeKey(ConnectionState state, int32_t packetId) {
			return (static_cast<uint64_t>(state) << 32) | static_cast<uint64_t>(static_cast<uint32_t>(packetId));
		}

		std::unordered_map<uint64_t, Factory> m_Factories;
	};

}
