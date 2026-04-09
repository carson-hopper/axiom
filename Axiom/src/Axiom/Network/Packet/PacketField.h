#pragma once

#include "Axiom/Network/Type/INetworkType.h"
#include "Axiom/Network/Type/Types.h"

#include <tuple>
#include <type_traits>
#include <utility>

namespace Axiom {

    template<typename T>
    struct Field {
        using FieldType = T;
        T Value{};

        Field() = default;
        Field(const T& value) : Value(value) {}
        Field(T&& value) : Value(std::move(value)) {}

        operator T&() { return Value; }
        operator const T&() const { return Value; }

        T* operator->() { return &Value; }
        const T* operator->() const { return &Value; }

        /**
         * Get the underlying value, unwrapping
         * NetworkType wrappers automatically.
         */
        auto GetValue() const {
            if constexpr (std::is_base_of_v<Net::INetworkType, T>) {
                return Value.GetValue();
            } else {
                return Value;
            }
        }
    };

#define AX_FIELD(Name, Type) \
    private: \
        Field<Type> m_##Name; \
    public: \
        auto Get##Name() const { return m_##Name.GetValue(); } \
        void Set##Name(const Type& value) { m_##Name.Value = value; }

#define AX_START_FIELDS() \
    public: \
        auto Fields() { return std::tie(

#define AX_DECLARE(Name) m_##Name

#define AX_END_FIELDS() \
        ); }

}
