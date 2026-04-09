#pragma once

#include <cstdint>
#include <string>

namespace Axiom {

class UUID {
public:
	UUID() : m_MostSignificant(0), m_LeastSignificant(0) {}

	UUID(uint64_t mostSignificant, uint64_t leastSignificant)
		: m_MostSignificant(mostSignificant)
		, m_LeastSignificant(leastSignificant) {}

	/** Parse from "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" or "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx". */
	static UUID FromString(const std::string& text) {
		std::string clean = text;
		std::erase(clean, '-');

		if (clean.size() != 32) return {};

		uint64_t most = 0;
		uint64_t least = 0;
		for (int i = 0; i < 16; i++) {
			char hexByte[3] = {clean[i * 2], clean[i * 2 + 1], '\0'};
			auto byte = static_cast<uint8_t>(std::strtoul(hexByte, nullptr, 16));
			if (i < 8) {
				most = (most << 8) | byte;
			} else {
				least = (least << 8) | byte;
			}
		}
		return {most, least};
	}

	/** Format as "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx". */
	std::string ToString() const {
		char buffer[37];
		uint8_t bytes[16];
		for (int i = 0; i < 8; i++) bytes[i] = static_cast<uint8_t>((m_MostSignificant >> (56 - i * 8)) & 0xFF);
		for (int i = 0; i < 8; i++) bytes[8 + i] = static_cast<uint8_t>((m_LeastSignificant >> (56 - i * 8)) & 0xFF);

		std::snprintf(buffer, sizeof(buffer),
			"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			bytes[0], bytes[1], bytes[2], bytes[3],
			bytes[4], bytes[5], bytes[6], bytes[7],
			bytes[8], bytes[9], bytes[10], bytes[11],
			bytes[12], bytes[13], bytes[14], bytes[15]);
		return buffer;
	}

	uint64_t MostSignificantBits() const { return m_MostSignificant; }
	uint64_t LeastSignificantBits() const { return m_LeastSignificant; }

	bool operator==(const UUID&) const = default;
	bool IsEmpty() const { return m_MostSignificant == 0 && m_LeastSignificant == 0; }

private:
	uint64_t m_MostSignificant;
	uint64_t m_LeastSignificant;
};

}

namespace std {
template<>
struct hash<Axiom::UUID> {
	std::size_t operator()(const Axiom::UUID& uuid) const noexcept {
		return std::hash<uint64_t>{}(uuid.MostSignificantBits())
			^ (std::hash<uint64_t>{}(uuid.LeastSignificantBits()) << 1);
	}
};
}
