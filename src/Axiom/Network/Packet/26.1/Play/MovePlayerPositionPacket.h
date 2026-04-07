#pragma once

#include "Axiom/Network/Packet/ServerboundPacket.h"
#include "Axiom/Network/Protocol.h"

namespace Axiom {

	template<int32_t Version = PROTOCOL_VERSION>
	class MovePlayerPositionPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::MovePlayerPosition;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			x = buffer.ReadDouble();
			y = buffer.ReadDouble();
			z = buffer.ReadDouble();
			flags = buffer.ReadByte();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		double x = 0;
		double y = 0;
		double z = 0;
		uint8_t flags = 0;
	};

	template<int32_t Version = PROTOCOL_VERSION>
	class MovePlayerPositionRotationPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::MovePlayerPositionRotation;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			x = buffer.ReadDouble();
			y = buffer.ReadDouble();
			z = buffer.ReadDouble();
			yaw = buffer.ReadFloat();
			pitch = buffer.ReadFloat();
			flags = buffer.ReadByte();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		double x = 0;
		double y = 0;
		double z = 0;
		float yaw = 0;
		float pitch = 0;
		uint8_t flags = 0;
	};

	template<int32_t Version = PROTOCOL_VERSION>
	class MovePlayerRotationPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::MovePlayerRotation;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			yaw = buffer.ReadFloat();
			pitch = buffer.ReadFloat();
			flags = buffer.ReadByte();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		float yaw = 0;
		float pitch = 0;
		uint8_t flags = 0;
	};

	template<int32_t Version = PROTOCOL_VERSION>
	class MovePlayerStatusOnlyPacket : public ServerboundPacket {
	public:
		static constexpr int32_t PacketId = Serverbound::Play::MovePlayerStatusOnly;
		static constexpr ConnectionState PacketState = ConnectionState::Play;

		void Decode(NetworkBuffer& buffer) override {
			flags = buffer.ReadByte();
		}

		void Handle(Ref<Connection> connection, PacketContext& context) override;

		uint8_t flags = 0;
	};

}
