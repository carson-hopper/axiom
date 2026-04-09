#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>
#include <vector>

namespace Axiom {

	class Connection;

	/**
	 * Groups multiple outbound packets into a single bundle
	 * packet (0x00 delimiter-based). Reduces per-packet
	 * framing overhead for bulk sends like initial
	 * chunk delivery and entity updates.
	 *
	 * Usage:
	 *   PacketBundler bundler;
	 *   bundler.Begin();
	 *   bundler.Add(packet1Buffer);
	 *   bundler.Add(packet2Buffer);
	 *   bundler.Flush(connection);
	 */
	class PacketBundler {
	public:
		/** Start accumulating packets for a bundle. */
		void Begin();

		/** Add a pre-serialized packet to the current bundle. */
		void Add(const std::vector<uint8_t>& packetData);
		void Add(const uint8_t* data, size_t length);

		/**
		 * Flush the bundle: sends bundle delimiter (0x00),
		 * all accumulated packets, then closing delimiter.
		 * If only one packet was added, sends it directly
		 * without bundling.
		 */
		void Flush(const Ref<Connection>& connection);

		/** Discard accumulated packets without sending. */
		void Clear();

		/** Number of packets in the current bundle. */
		size_t Count() const { return m_Packets.size(); }

		/** Whether a bundle is in progress. */
		bool IsActive() const { return m_Active; }

	private:
		std::vector<std::vector<uint8_t>> m_Packets;
		bool m_Active = false;
	};

}
