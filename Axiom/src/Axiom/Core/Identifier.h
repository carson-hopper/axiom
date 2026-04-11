#pragma once

#include "Axiom/Core/Base.h"

namespace Axiom {

	template<typename Tag>
	class Identifier {
	public:
		Identifier() : m_Value(s_Next++) {}
		explicit Identifier(const uint64_t value) : m_Value(value) {}

		uint64_t Value() const { return m_Value; }

		bool operator==(const Identifier&) const = default;
		auto operator<=>(const Identifier&) const = default;

	private:
		uint64_t m_Value;
		static inline std::atomic<uint64_t> s_Next{1};
	};

}

template<typename Tag>
	struct std::hash<Axiom::Identifier<Tag>> {
	std::size_t operator()(const Axiom::Identifier<Tag>& identifier) const noexcept {
		return std::hash<uint64_t>{}(identifier.Value());
	}
};