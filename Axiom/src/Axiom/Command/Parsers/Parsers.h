#pragma once

#include "Axiom/Command/Parsers/ArgumentParser.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace Axiom {

	/**
	 * All Brigadier argument parsers for protocol 775. Each parser
	 * writes the numeric ID from the command_argument_type registry
	 * plus any type-specific properties.
	 *
	 * Property formats:
	 * - brigadier:float/double/integer/long: flags byte + optional min/max
	 *     0x01 = has min, 0x02 = has max
	 * - brigadier:string: VarInt mode (0=single, 1=quotable, 2=greedy)
	 * - minecraft:entity: flags byte
	 *     0x01 = single target only, 0x02 = players only
	 * - minecraft:score_holder: flags byte (0x01 = allow multiple)
	 * - minecraft:resource/resource_key/resource_or_tag/resource_or_tag_key/resource_selector:
	 *     String registry identifier (e.g. "minecraft:block")
	 */

	// ----- Primitive parsers ------------------------------------------

	class BoolParser : public ArgumentParser {
	public:
		int32_t ParserId() const override { return 0; }
	};

	class FloatParser : public ArgumentParser {
	public:
		FloatParser() = default;
		FloatParser(std::optional<float> min, std::optional<float> max = std::nullopt)
			: m_Min(min), m_Max(max) {}

		int32_t ParserId() const override { return 1; }

		void WriteProperties(NetworkBuffer& buffer) const override {
			uint8_t flags = 0;
			if (m_Min.has_value()) flags |= 0x01;
			if (m_Max.has_value()) flags |= 0x02;
			buffer.WriteByte(flags);
			if (m_Min.has_value()) buffer.WriteFloat(m_Min.value());
			if (m_Max.has_value()) buffer.WriteFloat(m_Max.value());
		}

	private:
		std::optional<float> m_Min;
		std::optional<float> m_Max;
	};

	class DoubleParser : public ArgumentParser {
	public:
		DoubleParser() = default;
		DoubleParser(std::optional<double> min, std::optional<double> max = std::nullopt)
			: m_Min(min), m_Max(max) {}

		int32_t ParserId() const override { return 2; }

		void WriteProperties(NetworkBuffer& buffer) const override {
			uint8_t flags = 0;
			if (m_Min.has_value()) flags |= 0x01;
			if (m_Max.has_value()) flags |= 0x02;
			buffer.WriteByte(flags);
			if (m_Min.has_value()) buffer.WriteDouble(m_Min.value());
			if (m_Max.has_value()) buffer.WriteDouble(m_Max.value());
		}

	private:
		std::optional<double> m_Min;
		std::optional<double> m_Max;
	};

	class IntegerParser : public ArgumentParser {
	public:
		IntegerParser() = default;
		IntegerParser(std::optional<int32_t> min, std::optional<int32_t> max = std::nullopt)
			: m_Min(min), m_Max(max) {}

		int32_t ParserId() const override { return 3; }

		void WriteProperties(NetworkBuffer& buffer) const override {
			uint8_t flags = 0;
			if (m_Min.has_value()) flags |= 0x01;
			if (m_Max.has_value()) flags |= 0x02;
			buffer.WriteByte(flags);
			if (m_Min.has_value()) buffer.WriteInt(m_Min.value());
			if (m_Max.has_value()) buffer.WriteInt(m_Max.value());
		}

	private:
		std::optional<int32_t> m_Min;
		std::optional<int32_t> m_Max;
	};

	class LongParser : public ArgumentParser {
	public:
		LongParser() = default;
		LongParser(std::optional<int64_t> min, std::optional<int64_t> max = std::nullopt)
			: m_Min(min), m_Max(max) {}

		int32_t ParserId() const override { return 4; }

		void WriteProperties(NetworkBuffer& buffer) const override {
			uint8_t flags = 0;
			if (m_Min.has_value()) flags |= 0x01;
			if (m_Max.has_value()) flags |= 0x02;
			buffer.WriteByte(flags);
			if (m_Min.has_value()) buffer.WriteLong(m_Min.value());
			if (m_Max.has_value()) buffer.WriteLong(m_Max.value());
		}

	private:
		std::optional<int64_t> m_Min;
		std::optional<int64_t> m_Max;
	};

	class StringParser : public ArgumentParser {
	public:
		enum class Mode : int32_t {
			SingleWord = 0,
			QuotablePhrase = 1,
			GreedyPhrase = 2,
		};

		StringParser() = default;
		explicit StringParser(const Mode mode) : m_Mode(mode) {}

		int32_t ParserId() const override { return 5; }

		void WriteProperties(NetworkBuffer& buffer) const override {
			buffer.WriteVarInt(static_cast<int32_t>(m_Mode));
		}

	private:
		Mode m_Mode = Mode::SingleWord;
	};

	// ----- Entity / profile / position --------------------------------

	class EntityParser : public ArgumentParser {
	public:
		EntityParser() = default;
		EntityParser(bool singleTarget, bool playersOnly)
			: m_SingleTarget(singleTarget), m_PlayersOnly(playersOnly) {}

		int32_t ParserId() const override { return 6; }

		void WriteProperties(NetworkBuffer& buffer) const override {
			uint8_t flags = 0;
			if (m_SingleTarget) flags |= 0x01;
			if (m_PlayersOnly)  flags |= 0x02;
			buffer.WriteByte(flags);
		}

	private:
		bool m_SingleTarget = false;
		bool m_PlayersOnly  = false;
	};

	class GameProfileParser : public ArgumentParser {
	public:
		int32_t ParserId() const override { return 7; }
	};

	class BlockPosParser : public ArgumentParser {
	public:
		int32_t ParserId() const override { return 8; }
	};

	class ColumnPosParser : public ArgumentParser {
	public:
		int32_t ParserId() const override { return 9; }
	};

	class Vec3Parser : public ArgumentParser {
	public:
		int32_t ParserId() const override { return 10; }
	};

	class Vec2Parser : public ArgumentParser {
	public:
		int32_t ParserId() const override { return 11; }
	};

	// ----- Blocks / items ---------------------------------------------

	class BlockStateParser     : public ArgumentParser { public: int32_t ParserId() const override { return 12; } };
	class BlockPredicateParser : public ArgumentParser { public: int32_t ParserId() const override { return 13; } };
	class ItemStackParser      : public ArgumentParser { public: int32_t ParserId() const override { return 14; } };
	class ItemPredicateParser  : public ArgumentParser { public: int32_t ParserId() const override { return 15; } };

	// ----- Text / style / component -----------------------------------

	class ColorParser     : public ArgumentParser { public: int32_t ParserId() const override { return 16; } };
	class ComponentParser : public ArgumentParser { public: int32_t ParserId() const override { return 17; } };
	class StyleParser     : public ArgumentParser { public: int32_t ParserId() const override { return 18; } };
	class MessageParser   : public ArgumentParser { public: int32_t ParserId() const override { return 19; } };

	// ----- NBT ---------------------------------------------------------

	class NbtParser     : public ArgumentParser { public: int32_t ParserId() const override { return 20; } };
	class NbtTagParser  : public ArgumentParser { public: int32_t ParserId() const override { return 21; } };
	class NbtPathParser : public ArgumentParser { public: int32_t ParserId() const override { return 22; } };

	// ----- Objectives / scores ----------------------------------------

	class ObjectiveParser         : public ArgumentParser { public: int32_t ParserId() const override { return 23; } };
	class ObjectiveCriteriaParser : public ArgumentParser { public: int32_t ParserId() const override { return 24; } };
	class OperationParser         : public ArgumentParser { public: int32_t ParserId() const override { return 25; } };
	class ParticleParser          : public ArgumentParser { public: int32_t ParserId() const override { return 26; } };
	class AngleParser             : public ArgumentParser { public: int32_t ParserId() const override { return 27; } };
	class RotationParser          : public ArgumentParser { public: int32_t ParserId() const override { return 28; } };
	class ScoreboardSlotParser    : public ArgumentParser { public: int32_t ParserId() const override { return 29; } };

	class ScoreHolderParser : public ArgumentParser {
	public:
		ScoreHolderParser() = default;
		explicit ScoreHolderParser(const bool allowMultiple) : m_AllowMultiple(allowMultiple) {}

		int32_t ParserId() const override { return 30; }

		void WriteProperties(NetworkBuffer& buffer) const override {
			buffer.WriteByte(m_AllowMultiple ? 0x01 : 0x00);
		}

	private:
		bool m_AllowMultiple = false;
	};

	class SwizzleParser   : public ArgumentParser { public: int32_t ParserId() const override { return 31; } };
	class TeamParser      : public ArgumentParser { public: int32_t ParserId() const override { return 32; } };
	class ItemSlotParser  : public ArgumentParser { public: int32_t ParserId() const override { return 33; } };
	class ItemSlotsParser : public ArgumentParser { public: int32_t ParserId() const override { return 34; } };

	class ResourceLocationParser : public ArgumentParser { public: int32_t ParserId() const override { return 35; } };
	class FunctionParser         : public ArgumentParser { public: int32_t ParserId() const override { return 36; } };
	class EntityAnchorParser     : public ArgumentParser { public: int32_t ParserId() const override { return 37; } };
	class IntRangeParser         : public ArgumentParser { public: int32_t ParserId() const override { return 38; } };
	class FloatRangeParser       : public ArgumentParser { public: int32_t ParserId() const override { return 39; } };
	class DimensionParser        : public ArgumentParser { public: int32_t ParserId() const override { return 40; } };
	class GameModeParser         : public ArgumentParser { public: int32_t ParserId() const override { return 41; } };
	class TimeParser             : public ArgumentParser { public: int32_t ParserId() const override { return 42; } };

	// ----- Resource parsers (take a registry identifier) --------------

	class ResourceOrTagParser : public ArgumentParser {
	public:
		explicit ResourceOrTagParser(std::string registry) : m_Registry(std::move(registry)) {}
		int32_t ParserId() const override { return 43; }
		void WriteProperties(NetworkBuffer& buffer) const override { buffer.WriteString(m_Registry); }
	private:
		std::string m_Registry;
	};

	class ResourceOrTagKeyParser : public ArgumentParser {
	public:
		explicit ResourceOrTagKeyParser(std::string registry) : m_Registry(std::move(registry)) {}
		int32_t ParserId() const override { return 44; }
		void WriteProperties(NetworkBuffer& buffer) const override { buffer.WriteString(m_Registry); }
	private:
		std::string m_Registry;
	};

	class ResourceParser : public ArgumentParser {
	public:
		explicit ResourceParser(std::string registry) : m_Registry(std::move(registry)) {}
		int32_t ParserId() const override { return 45; }
		void WriteProperties(NetworkBuffer& buffer) const override { buffer.WriteString(m_Registry); }
	private:
		std::string m_Registry;
	};

	class ResourceKeyParser : public ArgumentParser {
	public:
		explicit ResourceKeyParser(std::string registry) : m_Registry(std::move(registry)) {}
		int32_t ParserId() const override { return 46; }
		void WriteProperties(NetworkBuffer& buffer) const override { buffer.WriteString(m_Registry); }
	private:
		std::string m_Registry;
	};

	class ResourceSelectorParser : public ArgumentParser {
	public:
		explicit ResourceSelectorParser(std::string registry) : m_Registry(std::move(registry)) {}
		int32_t ParserId() const override { return 47; }
		void WriteProperties(NetworkBuffer& buffer) const override { buffer.WriteString(m_Registry); }
	private:
		std::string m_Registry;
	};

	// ----- Misc --------------------------------------------------------

	class TemplateMirrorParser   : public ArgumentParser { public: int32_t ParserId() const override { return 48; } };
	class TemplateRotationParser : public ArgumentParser { public: int32_t ParserId() const override { return 49; } };
	class HeightmapParser        : public ArgumentParser { public: int32_t ParserId() const override { return 50; } };
	class LootTableParser        : public ArgumentParser { public: int32_t ParserId() const override { return 51; } };
	class LootPredicateParser    : public ArgumentParser { public: int32_t ParserId() const override { return 52; } };
	class LootModifierParser     : public ArgumentParser { public: int32_t ParserId() const override { return 53; } };
	class DialogParser           : public ArgumentParser { public: int32_t ParserId() const override { return 54; } };
	class UuidParser             : public ArgumentParser { public: int32_t ParserId() const override { return 55; } };

}
