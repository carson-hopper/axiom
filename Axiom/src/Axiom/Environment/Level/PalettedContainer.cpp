#include "Axiom/Environment/Level/PalettedContainer.h"

#include <algorithm>
#include <unordered_set>

namespace Axiom {

	PalettedContainer::PalettedContainer()
		: m_Mode(PaletteMode::SingleValue)
		, m_SingleValue(0)
		, m_BitsPerEntry(0) {
	}

	PalettedContainer::PalettedContainer(int32_t defaultValue)
		: m_Mode(PaletteMode::SingleValue)
		, m_SingleValue(defaultValue)
		, m_BitsPerEntry(0) {
	}

	int PalettedContainer::IndexFor(int x, int y, int z) const {
		return (y * SECTION_SIZE + z) * SECTION_SIZE + x;
	}

	int PalettedContainer::EntriesPerLong() const {
		if (m_BitsPerEntry == 0) return 0;
		return 64 / m_BitsPerEntry;
	}

	int PalettedContainer::DataArrayLength() const {
		if (m_BitsPerEntry == 0) return 0;
		const int perLong = EntriesPerLong();
		return (TOTAL_ENTRIES + perLong - 1) / perLong;
	}

	void PalettedContainer::PackEntry(int index, uint64_t value) {
		const int perLong = EntriesPerLong();
		const int longIndex = index / perLong;
		const int bitOffset = (index % perLong) * m_BitsPerEntry;
		const uint64_t mask = (static_cast<uint64_t>(1) << m_BitsPerEntry) - 1;

		m_Data[longIndex] &= ~(mask << bitOffset);
		m_Data[longIndex] |= (value & mask) << bitOffset;
	}

	uint64_t PalettedContainer::UnpackEntry(int index) const {
		const int perLong = EntriesPerLong();
		const int longIndex = index / perLong;
		const int bitOffset = (index % perLong) * m_BitsPerEntry;
		const uint64_t mask = (static_cast<uint64_t>(1) << m_BitsPerEntry) - 1;

		return (m_Data[longIndex] >> bitOffset) & mask;
	}

	int32_t PalettedContainer::Get(int x, int y, int z) const {
		if (m_Mode == PaletteMode::SingleValue) {
			return m_SingleValue;
		}

		const int entryIndex = IndexFor(x, y, z);
		const uint64_t rawValue = UnpackEntry(entryIndex);

		if (m_Mode == PaletteMode::Global) {
			return static_cast<int32_t>(rawValue);
		}

		// Linear or Hashmap: rawValue is palette index
		const auto paletteIndex = static_cast<int>(rawValue);
		if (paletteIndex < 0 || paletteIndex >= static_cast<int>(m_Palette.size())) {
			return 0;
		}
		return m_Palette[paletteIndex];
	}

	void PalettedContainer::Set(int x, int y, int z, int32_t value) {
		if (m_Mode == PaletteMode::SingleValue) {
			if (value == m_SingleValue) return;
			PromoteToLinear();
		}

		if (m_Mode == PaletteMode::Global) {
			const int entryIndex = IndexFor(x, y, z);
			PackEntry(entryIndex, static_cast<uint64_t>(value));
			return;
		}

		// Linear or Hashmap: find or add palette entry.
		// FindOrAddPaletteEntry may promote us to Global
		// mid-call (Hashmap overflow). In that case the
		// value we're setting was NOT added to any palette
		// and the return value is a -1 sentinel — we must
		// write `value` as a raw block state ID instead of
		// the sentinel, or the caller's block is silently
		// corrupted to the 15-bit truncation of -1 (32767).
		const int paletteIndex = FindOrAddPaletteEntry(value);
		const int entryIndex = IndexFor(x, y, z);

		if (m_Mode == PaletteMode::Global) {
			PackEntry(entryIndex, static_cast<uint64_t>(value));
			return;
		}

		PackEntry(entryIndex, static_cast<uint64_t>(paletteIndex));
	}

	int PalettedContainer::FindOrAddPaletteEntry(int32_t value) {
		for (int searchIndex = 0; searchIndex < static_cast<int>(m_Palette.size()); searchIndex++) {
			if (m_Palette[searchIndex] == value) {
				return searchIndex;
			}
		}

		// Need to add a new entry -- check if promotion is required
		if (m_Mode == PaletteMode::Linear && static_cast<int>(m_Palette.size()) >= LINEAR_MAX_ENTRIES) {
			PromoteToHashmap();
			return FindOrAddPaletteEntry(value);
		}

		if (m_Mode == PaletteMode::Hashmap && static_cast<int>(m_Palette.size()) >= HASHMAP_MAX_ENTRIES) {
			PromoteToGlobal();
			// After promoting to Global, the caller will use direct IDs.
			// Return -1 as a signal; the caller (Set) handles Global mode
			// before reaching FindOrAddPaletteEntry, so this path only
			// occurs during promotion cascades.
			return -1;
		}

		const int newIndex = static_cast<int>(m_Palette.size());
		m_Palette.push_back(value);
		return newIndex;
	}

	void PalettedContainer::PromoteToLinear() {
		// Transitioning from SingleValue to Linear
		m_Mode = PaletteMode::Linear;
		m_BitsPerEntry = LINEAR_BITS_PER_ENTRY;

		m_Palette.clear();
		m_Palette.push_back(m_SingleValue);

		m_Data.assign(DataArrayLength(), 0);
		// All entries are palette index 0 (the single value), so data stays zeroed
	}

	void PalettedContainer::PromoteToHashmap() {
		// Transitioning from Linear to Hashmap -- same palette, wider bits
		const std::vector<int32_t> oldPalette = m_Palette;
		const int oldBitsPerEntry = m_BitsPerEntry;
		const std::vector<uint64_t> oldData = m_Data;

		m_Mode = PaletteMode::Hashmap;
		m_BitsPerEntry = HASHMAP_BITS_PER_ENTRY;
		m_Data.assign(DataArrayLength(), 0);

		// Re-pack all entries with wider bit width
		const int oldPerLong = 64 / oldBitsPerEntry;
		const uint64_t oldMask = (static_cast<uint64_t>(1) << oldBitsPerEntry) - 1;

		for (int entryIndex = 0; entryIndex < TOTAL_ENTRIES; entryIndex++) {
			const int longIndex = entryIndex / oldPerLong;
			const int bitOffset = (entryIndex % oldPerLong) * oldBitsPerEntry;
			const uint64_t paletteIndex = (oldData[longIndex] >> bitOffset) & oldMask;
			PackEntry(entryIndex, paletteIndex);
		}
	}

	void PalettedContainer::PromoteToGlobal() {
		// Transitioning from Hashmap (or Linear) to Global -- expand palette indices to real IDs
		const std::vector<int32_t> oldPalette = m_Palette;
		const int oldBitsPerEntry = m_BitsPerEntry;
		const std::vector<uint64_t> oldData = m_Data;

		m_Mode = PaletteMode::Global;
		m_BitsPerEntry = GLOBAL_BITS_PER_ENTRY;
		m_Palette.clear();
		m_Data.assign(DataArrayLength(), 0);

		const int oldPerLong = 64 / oldBitsPerEntry;
		const uint64_t oldMask = (static_cast<uint64_t>(1) << oldBitsPerEntry) - 1;

		for (int entryIndex = 0; entryIndex < TOTAL_ENTRIES; entryIndex++) {
			const int longIndex = entryIndex / oldPerLong;
			const int bitOffset = (entryIndex % oldPerLong) * oldBitsPerEntry;
			const uint64_t paletteIndex = (oldData[longIndex] >> bitOffset) & oldMask;

			int32_t blockStateId = 0;
			if (static_cast<int>(paletteIndex) < static_cast<int>(oldPalette.size())) {
				blockStateId = oldPalette[static_cast<int>(paletteIndex)];
			}
			PackEntry(entryIndex, static_cast<uint64_t>(blockStateId));
		}
	}

	void PalettedContainer::Fill(int32_t value) {
		m_Mode = PaletteMode::SingleValue;
		m_SingleValue = value;
		m_BitsPerEntry = 0;
		m_Palette.clear();
		m_Data.clear();
	}

	int PalettedContainer::UniqueCount() const {
		if (m_Mode == PaletteMode::SingleValue) {
			return 1;
		}

		if (m_Mode == PaletteMode::Linear || m_Mode == PaletteMode::Hashmap) {
			return static_cast<int>(m_Palette.size());
		}

		// Global mode: scan all entries
		std::unordered_set<int32_t> uniqueValues;
		for (int entryIndex = 0; entryIndex < TOTAL_ENTRIES; entryIndex++) {
			const auto blockStateId = static_cast<int32_t>(UnpackEntry(entryIndex));
			uniqueValues.insert(blockStateId);
		}
		return static_cast<int>(uniqueValues.size());
	}

	int PalettedContainer::NonAirCount() const {
		if (m_Mode == PaletteMode::SingleValue) {
			return m_SingleValue != 0 ? TOTAL_ENTRIES : 0;
		}

		int count = 0;
		for (int entryIndex = 0; entryIndex < TOTAL_ENTRIES; entryIndex++) {
			int32_t blockStateId = 0;

			if (m_Mode == PaletteMode::Global) {
				blockStateId = static_cast<int32_t>(UnpackEntry(entryIndex));
			} else {
				const auto paletteIndex = static_cast<int>(UnpackEntry(entryIndex));
				if (paletteIndex >= 0 && paletteIndex < static_cast<int>(m_Palette.size())) {
					blockStateId = m_Palette[paletteIndex];
				}
			}

			if (blockStateId != 0) {
				count++;
			}
		}
		return count;
	}

	void PalettedContainer::WriteToNetwork(NetworkBuffer& buffer) const {
		buffer.WriteByte(static_cast<uint8_t>(m_BitsPerEntry));

		if (m_Mode == PaletteMode::SingleValue) {
			// Single value palette: write just the value, no data array
			buffer.WriteVarInt(m_SingleValue);
			buffer.WriteVarInt(0); // data array length = 0
			return;
		}

		if (m_Mode == PaletteMode::Linear || m_Mode == PaletteMode::Hashmap) {
			// Indirect palette: write palette size, then entries
			buffer.WriteVarInt(static_cast<int32_t>(m_Palette.size()));
			for (const int32_t paletteEntry : m_Palette) {
				buffer.WriteVarInt(paletteEntry);
			}
		}
		// Global mode: no palette written

		// Data array
		buffer.WriteVarInt(static_cast<int32_t>(m_Data.size()));
		for (const uint64_t dataLong : m_Data) {
			buffer.WriteLong(static_cast<int64_t>(dataLong));
		}
	}

	void PalettedContainer::ReadFromNetwork(NetworkBuffer& buffer) {
		const uint8_t bitsPerEntry = buffer.ReadByte();

		if (bitsPerEntry == 0) {
			// Single value mode
			m_Mode = PaletteMode::SingleValue;
			m_BitsPerEntry = 0;
			m_SingleValue = buffer.ReadVarInt();
			m_Palette.clear();

			const int32_t dataLength = buffer.ReadVarInt();
			m_Data.clear();
			for (int32_t longIndex = 0; longIndex < dataLength; longIndex++) {
				m_Data.push_back(static_cast<uint64_t>(buffer.ReadLong()));
			}
			return;
		}

		m_BitsPerEntry = bitsPerEntry;

		if (bitsPerEntry <= LINEAR_BITS_PER_ENTRY) {
			m_Mode = PaletteMode::Linear;
		} else if (bitsPerEntry <= HASHMAP_BITS_PER_ENTRY) {
			m_Mode = PaletteMode::Hashmap;
		} else {
			m_Mode = PaletteMode::Global;
		}

		// Read palette for indirect modes
		if (m_Mode == PaletteMode::Linear || m_Mode == PaletteMode::Hashmap) {
			const int32_t paletteLength = buffer.ReadVarInt();
			m_Palette.clear();
			m_Palette.reserve(paletteLength);
			for (int32_t paletteIndex = 0; paletteIndex < paletteLength; paletteIndex++) {
				m_Palette.push_back(buffer.ReadVarInt());
			}
		} else {
			m_Palette.clear();
		}

		// Read data array
		const int32_t dataLength = buffer.ReadVarInt();
		m_Data.clear();
		m_Data.reserve(dataLength);
		for (int32_t longIndex = 0; longIndex < dataLength; longIndex++) {
			m_Data.push_back(static_cast<uint64_t>(buffer.ReadLong()));
		}
	}

}
