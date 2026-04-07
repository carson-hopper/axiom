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
#include <map>
#include <unordered_map>

namespace Axiom {

	/**
	 * Single packet registry supporting multiple protocol versions.
	 *
	 * Register packets with a version number. On dispatch, the registry
	 * finds the best match: highest registered version <= the connection's
	 * protocol version. When a new MC version ships, only register
	 * changed packets — everything else falls back automatically.
	 *
	 * Usage:
	 *   PacketRegistry registry;
	 *   registry.Register<775, HandshakePacket<775>>();
	 *   registry.Register<775, StatusRequestPacket<775>>();
	 *   // MC 26.2 ships — only register what changed:
	 *   registry.Register<776, SomeChangedPacket<776>>();
	 *   // Unchanged packets fall back to 775 version
	 *
	 * Dispatch auto-fires PacketReceiveEvent (cancellable) before Handle()
	 * and PacketHandledEvent after Handle().
	 */
	class PacketRegistry {
	public:
		using Factory = std::function<Scope<ServerboundPacket>()>;

		/**
		 * Register a packet type for a specific protocol version.
		 * Reads PacketId and PacketState from the type's static members.
		 */
		template<int32_t Version, typename T>
		void Register() {
			auto statePacketKey = MakeStatePacketKey(T::PacketState, T::PacketId);
			m_Factories[statePacketKey][Version] = []() -> Scope<ServerboundPacket> {
				return CreateScope<T>();
			};
		}

		/**
		 * Create a packet instance for the given version, state, and ID.
		 * Falls back to the highest registered version <= protocolVersion.
		 * Returns nullptr if no suitable packet is registered.
		 */
		Scope<ServerboundPacket> Create(const int32_t protocolVersion, const ConnectionState state, const int32_t packetId) const {

			const auto statePacketKey = MakeStatePacketKey(state, packetId);
			const auto outerIterator = m_Factories.find(statePacketKey);
			if (outerIterator == m_Factories.end()) {
				return nullptr;
			}

			// Find highest version <= protocolVersion
			const auto& versionMap = outerIterator->second;
			auto versionIterator = versionMap.upper_bound(protocolVersion);
			if (versionIterator == versionMap.begin()) {
				return nullptr;  // All registered versions are newer
			}
			--versionIterator;

			return versionIterator->second();
		}

		/**
		 * Create, decode, fire pre-event, handle, fire post-event.
		 * Uses the connection's protocol version for packet selection.
		 * Returns false if no packet registered for this version/state/id.
		 */
		bool Dispatch(ConnectionState state, int32_t packetId, NetworkBuffer& buffer, const Ref<Connection>& connection, PacketContext& context) const {

			int32_t protocolVersion = connection->ProtocolVersion();

			// Handshake packets arrive before we know the version — use current
			if (state == ConnectionState::Handshake) {
				protocolVersion = PROTOCOL_VERSION;
			}

			auto packet = Create(protocolVersion, state, packetId);
			if (!packet) {
				AX_CORE_WARN("Unhandled packet 0x{:02X} in state {} for protocol {}",
					packetId, static_cast<int>(state), protocolVersion);
				return false;
			}

			packet->Decode(buffer);

			PacketReceiveEvent receiveEvent(connection, state, packetId, packet.get());
			context.Events().Publish(receiveEvent);

			if (receiveEvent.m_Handled) {
				return true;
			}

			packet->Handle(connection, context);

			PacketHandledEvent handledEvent(connection, state, packetId, packet.get());
			context.Events().Publish(handledEvent);

			return true;
		}

	private:
		static uint64_t MakeStatePacketKey(ConnectionState state, int32_t packetId) {
			return (static_cast<uint64_t>(state) << 32)
				| static_cast<uint64_t>(static_cast<uint32_t>(packetId));
		}

		// (state, packetId) → ordered map of (version → factory)
		std::unordered_map<uint64_t, std::map<int32_t, Factory>> m_Factories;
	};

}
