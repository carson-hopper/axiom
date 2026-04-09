#include "Axiom/Network/PacketBundler.h"
#include "Axiom/Network/Connection.h"

namespace Axiom {

	void PacketBundler::Begin() {
		m_Packets.clear();
		m_Active = true;
	}

	void PacketBundler::Add(const std::vector<uint8_t>& packetData) {
		m_Packets.push_back(packetData);
	}

	void PacketBundler::Add(const uint8_t* data, size_t length) {
		m_Packets.emplace_back(data, data + length);
	}

	void PacketBundler::Flush(const Ref<Connection>& connection) {
		if (!m_Active) {
			return;
		}

		if (m_Packets.empty()) {
			Clear();
			return;
		}

		if (m_Packets.size() == 1) {
			/**
			 * Single packet -- send directly without
			 * bundle framing to avoid unnecessary
			 * delimiter overhead.
			 */
			NetworkBuffer payload(m_Packets[0]);
			int32_t packetId = payload.ReadVarInt();
			NetworkBuffer body(payload.ReadRemainingBytes());
			connection->SendRawPacket(packetId, body);
			Clear();
			return;
		}

		/**
		 * Multiple packets -- wrap in bundle delimiters.
		 * Send opening 0x00, then each packet, then
		 * closing 0x00 to mark the bundle boundary.
		 */
		NetworkBuffer emptyPayload;
		connection->SendRawPacket(0x00, emptyPayload);

		for (const auto& packetData : m_Packets) {
			NetworkBuffer payload(packetData);
			int32_t packetId = payload.ReadVarInt();
			NetworkBuffer body(payload.ReadRemainingBytes());
			connection->SendRawPacket(packetId, body);
		}

		connection->SendRawPacket(0x00, emptyPayload);

		Clear();
	}

	void PacketBundler::Clear() {
		m_Packets.clear();
		m_Active = false;
	}

}
