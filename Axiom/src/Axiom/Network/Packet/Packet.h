#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Math.h"
#include "Axiom/Network/Packet/IChainablePacket.h"
#include "Axiom/Network/Packet/PacketState.h"
#include "Axiom/Network/Packet/PacketDirection.h"
#include "Axiom/Network/Packet/PacketField.h"
#include "Axiom/Network/Packet/PacketIds.generated.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace Axiom {

    /**
     * Helper to build a chain of response packets from
     * variadic template types, each default-constructed.
     */
    template<typename... Ts>
    std::vector<Ref<IChainablePacket>> CreateChainPackets() {
        return { std::static_pointer_cast<IChainablePacket>(CreateRef<Ts>())... };
    }

    /**
     * Construct a single packet from a forwarded tuple of
     * constructor arguments and wrap it in a chain entry.
     */
    template<typename T, typename Tuple>
    Ref<IChainablePacket> ConstructPacketFromTuple(Tuple&& argumentTuple) {
        return std::apply([](auto&&... arguments) {
            return std::static_pointer_cast<IChainablePacket>(
                CreateRef<T>(std::forward<decltype(arguments)>(arguments)...)
            );
        }, std::forward<Tuple>(argumentTuple));
    }

    /**
     * Build a chain where each packet type receives a
     * corresponding tuple of constructor arguments.
     */
    template<typename... Ts, typename... Tuples>
    std::vector<Ref<IChainablePacket>> CreateChainPacketsWithArgs(Tuples&&... argumentTuples) {
        static_assert(sizeof...(Ts) == sizeof...(Tuples),
            "Each packet type must have a corresponding argument tuple.");
        return { ConstructPacketFromTuple<Ts>(std::forward<Tuples>(argumentTuples))... };
    }

    /**
     * CRTP base for all Minecraft packets.
     *
     * Derived classes declare fields via AX_FIELD / AX_START_FIELDS
     * macros and implement a Handle() method that returns an optional
     * chain of response packets.
     *
     * Template params:
     *   Derived   - the concrete packet class (CRTP)
     *   PacketId  - numeric packet ID from PID_* defines
     *   Direction - Serverbound or Clientbound
     *   State     - protocol state this packet belongs to
     */
    template<typename Derived, uint16_t PacketId,
             PacketDirection Direction, PacketState State>
    class Packet : public IChainablePacket {
    public:
        static constexpr uint16_t GetPacketIdStatic() { return PacketId; }
        static constexpr PacketDirection GetDirectionStatic() { return Direction; }
        static constexpr PacketState GetStateStatic() { return State; }

        int32_t GetPacketId() const override { return PacketId; }
        PacketDirection GetDirection() const override { return Direction; }

        /**
         * Deserialise every declared field from the buffer
         * in declaration order.
         */
        void Parse(NetworkBuffer& buffer) {
            std::apply([&](auto&... field) {
                (ReadField(buffer, field), ...);
            }, static_cast<Derived*>(this)->Fields());
        }

        /**
         * Serialise every declared field into the buffer
         * in declaration order.
         */
        void Write(NetworkBuffer& buffer) override {
            std::apply([&](auto&... field) {
                (WriteField(buffer, field), ...);
            }, static_cast<Derived*>(this)->Fields());
        }

        /**
         * Parse the buffer then delegate to the derived
         * Handle(), returning any chained response packets.
         */
        std::optional<std::vector<Ref<IChainablePacket>>>
        HandleImpl(const Ref<Connection>& connection,
                   PacketContext& context,
                   NetworkBuffer& buffer) override {
            Parse(buffer);
            return static_cast<Derived*>(this)->Handle(connection, context, buffer);
        }

    protected:
        template<typename F>
        void ReadField(NetworkBuffer& buffer, F& field) {
            using T = typename F::FieldType;

            if constexpr (std::is_base_of_v<Net::INetworkType, T>) {
                field.Value.Read(buffer);
            }
            else if constexpr (std::is_same_v<T, int8_t>) {
                field.Value = static_cast<int8_t>(buffer.ReadByte());
            }
            else if constexpr (std::is_same_v<T, uint8_t>) {
                field.Value = buffer.ReadByte();
            }
            else if constexpr (std::is_same_v<T, int16_t>) {
                field.Value = buffer.ReadShort();
            }
            else if constexpr (std::is_same_v<T, uint16_t>) {
                field.Value = buffer.ReadUnsignedShort();
            }
            else if constexpr (std::is_same_v<T, int32_t>) {
                field.Value = buffer.ReadVarInt();
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                field.Value = buffer.ReadLong();
            }
            else if constexpr (std::is_same_v<T, float>) {
                field.Value = buffer.ReadFloat();
            }
            else if constexpr (std::is_same_v<T, double>) {
                field.Value = buffer.ReadDouble();
            }
            else if constexpr (std::is_same_v<T, bool>) {
                field.Value = buffer.ReadBoolean();
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                field.Value = buffer.ReadString();
            }
            else if constexpr (std::is_same_v<T, Vector3>) {
                field.Value = buffer.ReadVector3();
            }
            else if constexpr (std::is_same_v<T, Vector2>) {
                field.Value = buffer.ReadVector2();
            }
            else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                field.Value = buffer.ReadRemainingBytes();
            }
        }

        template<typename F>
        void WriteField(NetworkBuffer& buffer, const F& field) {
            using T = typename F::FieldType;

            if constexpr (std::is_base_of_v<Net::INetworkType, T>) {
                field.Value.Write(buffer);
            }
            else if constexpr (std::is_same_v<T, int8_t>) {
                buffer.WriteByte(static_cast<uint8_t>(field.Value));
            }
            else if constexpr (std::is_same_v<T, uint8_t>) {
                buffer.WriteByte(field.Value);
            }
            else if constexpr (std::is_same_v<T, int16_t>) {
                buffer.WriteShort(field.Value);
            }
            else if constexpr (std::is_same_v<T, uint16_t>) {
                buffer.WriteUnsignedShort(field.Value);
            }
            else if constexpr (std::is_same_v<T, int32_t>) {
                buffer.WriteVarInt(field.Value);
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                buffer.WriteLong(field.Value);
            }
            else if constexpr (std::is_same_v<T, float>) {
                buffer.WriteFloat(field.Value);
            }
            else if constexpr (std::is_same_v<T, double>) {
                buffer.WriteDouble(field.Value);
            }
            else if constexpr (std::is_same_v<T, bool>) {
                buffer.WriteBoolean(field.Value);
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                buffer.WriteString(field.Value);
            }
            else if constexpr (std::is_same_v<T, Vector3>) {
                buffer.WriteVector3(field.Value);
            }
            else if constexpr (std::is_same_v<T, Vector2>) {
                buffer.WriteVector2(field.Value);
            }
            else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                buffer.WriteBytes(field.Value);
            }
        }
    };

}
