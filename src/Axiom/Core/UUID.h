#pragma once

namespace Axiom {

	class UUID
	{
	public:
		UUID();
		explicit UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		explicit operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

}

namespace std {
	template<>
	struct hash<Axiom::UUID>
	{
		std::size_t operator()(const Axiom::UUID& uuid) const noexcept
		{
			return (uint64_t)uuid;
		}
	};

}