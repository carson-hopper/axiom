#pragma once

#include "Axiom/Core/Base.h"

#include <cstring>
#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Reads Named Binary Tag (NBT) data from a byte buffer.
	 * Parses the vanilla chunk NBT format for block/biome extraction.
	 */
	class NbtReader {
	public:
		enum TagType : uint8_t {
			TagEnd = 0, TagByte = 1, TagShort = 2, TagInt = 3,
			TagLong = 4, TagFloat = 5, TagDouble = 6, TagByteArray = 7,
			TagString = 8, TagList = 9, TagCompound = 10,
			TagIntArray = 11, TagLongArray = 12
		};

		explicit NbtReader(const std::vector<uint8_t>& data)
			: m_Data(data.data()), m_Size(data.size()) {}

		NbtReader(const uint8_t* data, const size_t size)
			: m_Data(data), m_Size(size) {}

		// ----- Navigation -----------------------------------------------

		bool ReadRootCompound() {
			const uint8_t type = ReadByte();
			if (type != TagCompound) return false;
			const std::string name = ReadTagString();  // Root name (often empty in MC)
			return true;
		}

		/**
		 * Find a named tag within the current compound.
		 * Returns the tag type, or TagEnd if not found.
		 */
		TagType FindTag(const std::string& targetName) {
			const size_t savedPosition = m_Position;

			while (m_Position < m_Size) {
				const uint8_t type = ReadByte();
				if (type == TagEnd) {
					m_Position = savedPosition;
					return TagEnd;
				}

				const std::string name = ReadTagString();
				if (name == targetName) {
					return static_cast<TagType>(type);
				}

				SkipTag(static_cast<TagType>(type));
			}

			m_Position = savedPosition;
			return TagEnd;
		}

		/**
		 * Enter a compound tag (after FindTag returned TagCompound).
		 * Just changes parsing context — compound contents follow.
		 */
		void EnterCompound() {
			// Already positioned at the compound's first child tag
		}

		void SkipCompound() {
			while (m_Position < m_Size) {
				const uint8_t type = ReadByte();
				if (type == TagEnd) return;
				ReadTagString(); // Skip name
				SkipTag(static_cast<TagType>(type));
			}
		}

		// ----- List navigation ------------------------------------------

		struct ListHeader {
			TagType elementType;
			int32_t count;
		};

		ListHeader ReadListHeader() {
			const uint8_t elementType = ReadByte();
			const int32_t count = ReadInt();
			return {static_cast<TagType>(elementType), count};
		}

		// ----- Primitive readers ----------------------------------------

		int8_t ReadTagByte() { return static_cast<int8_t>(ReadByte()); }

		int16_t ReadTagShort() {
			const uint8_t high = ReadByte();
			const uint8_t low = ReadByte();
			return static_cast<int16_t>((high << 8) | low);
		}

		int32_t ReadInt() {
			int32_t value = 0;
			for (int i = 0; i < 4; i++) {
				value = (value << 8) | ReadByte();
			}
			return value;
		}

		int64_t ReadLong() {
			int64_t value = 0;
			for (int i = 0; i < 8; i++) {
				value = (value << 8) | ReadByte();
			}
			return value;
		}

		float ReadFloat() {
			const int32_t bits = ReadInt();
			float value;
			std::memcpy(&value, &bits, sizeof(float));
			return value;
		}

		double ReadDouble() {
			const int64_t bits = ReadLong();
			double value;
			std::memcpy(&value, &bits, sizeof(double));
			return value;
		}

		std::string ReadTagString() {
			const uint16_t length = static_cast<uint16_t>((ReadByte() << 8) | ReadByte());
			std::string result(length, '\0');
			for (uint16_t i = 0; i < length; i++) {
				result[i] = static_cast<char>(ReadByte());
			}
			return result;
		}

		std::vector<int64_t> ReadLongArray() {
			const int32_t count = ReadInt();
			std::vector<int64_t> result(count);
			for (int32_t i = 0; i < count; i++) {
				result[i] = ReadLong();
			}
			return result;
		}

		std::vector<int8_t> ReadByteArray() {
			const int32_t count = ReadInt();
			std::vector<int8_t> result(count);
			for (int32_t i = 0; i < count; i++) {
				result[i] = ReadTagByte();
			}
			return result;
		}

		size_t Position() const { return m_Position; }
		void SetPosition(const size_t position) { m_Position = position; }
		bool HasRemaining() const { return m_Position < m_Size; }

	private:
		uint8_t ReadByte() {
			if (m_Position >= m_Size) return 0;
			return m_Data[m_Position++];
		}

		void SkipTag(const TagType type) {
			switch (type) {
				case TagEnd: break;
				case TagByte: m_Position += 1; break;
				case TagShort: m_Position += 2; break;
				case TagInt: m_Position += 4; break;
				case TagLong: m_Position += 8; break;
				case TagFloat: m_Position += 4; break;
				case TagDouble: m_Position += 8; break;
				case TagByteArray: { const int32_t len = ReadInt(); m_Position += len; break; }
				case TagString: { ReadTagString(); break; }
				case TagList: {
					const auto header = ReadListHeader();
					for (int32_t i = 0; i < header.count; i++) {
						SkipTag(header.elementType);
					}
					break;
				}
				case TagCompound: { SkipCompound(); break; }
				case TagIntArray: { const int32_t len = ReadInt(); m_Position += len * 4; break; }
				case TagLongArray: { const int32_t len = ReadInt(); m_Position += len * 8; break; }
			}
		}

		const uint8_t* m_Data;
		size_t m_Size;
		size_t m_Position = 0;
	};

}
