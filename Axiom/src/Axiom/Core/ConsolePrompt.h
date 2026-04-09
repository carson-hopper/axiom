#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace Axiom {

/**
 * Calculate the visible terminal column width of a UTF-8 string.
 * Emojis (4-byte sequences) = 2 columns.
 * Other multi-byte characters = 1 column.
 * ASCII = 1 column.
 */
inline int ColumnWidth(const std::string& text) {
	int columns = 0;
	for (size_t i = 0; i < text.size(); ) {
		auto byte = static_cast<uint8_t>(text[i]);
		if (byte < 0x80) {
			columns += 1;
			i += 1;
		} else if ((byte & 0xE0) == 0xC0) {
			columns += 1;
			i += 2;
		} else if ((byte & 0xF0) == 0xE0) {
			// 3-byte: CJK, powerline glyphs — 1 column for private use area, 2 for CJK
			uint32_t codepoint = (static_cast<uint32_t>(byte & 0x0F) << 12)
				| (static_cast<uint32_t>(static_cast<uint8_t>(text[i + 1]) & 0x3F) << 6)
				| static_cast<uint32_t>(static_cast<uint8_t>(text[i + 2]) & 0x3F);
			// Private Use Area (powerline glyphs E0A0-E0FF): 1 column
			// CJK ranges: 2 columns
			if (codepoint >= 0xE000 && codepoint <= 0xF8FF) {
				columns += 1;
			} else if (codepoint >= 0x2E80) {
				columns += 2;
			} else {
				columns += 1;
			}
			i += 3;
		} else if ((byte & 0xF8) == 0xF0) {
			// 4-byte: emojis — 2 columns
			columns += 2;
			i += 4;
		} else {
			columns += 1;
			i += 1;
		}
	}
	return columns;
}

/**
 * Starship-style console prompt with colored segments.
 * Builds a two-line prompt:
 *   Line 1: left segments ... right segments
 *   Line 2: prompt character for input
 */
class ConsolePrompt {
public:
	struct Segment {
		std::string Icon;
		std::string Text;
		int ForegroundColor; // 256-color code
		int BackgroundColor; // 256-color code, -1 for none
	};

	using SegmentProvider = std::function<Segment()>;

	/** Add a segment to the left side. */
	void AddLeft(SegmentProvider provider) {
		m_LeftProviders.push_back(std::move(provider));
	}

	/** Add a segment to the right side. */
	void AddRight(SegmentProvider provider) {
		m_RightProviders.push_back(std::move(provider));
	}

	/** Set the prompt character (default: ❯). */
	void SetPromptChar(const std::string& character) {
		m_PromptChar = character;
	}

	/** Set prompt character color. */
	void SetPromptColor(int color) {
		m_PromptColor = color;
	}

	/**
	 * Build the full two-line prompt string.
	 * Returns {styledPrompt, visibleLength of line 2}.
	 */
	struct BuiltPrompt {
		std::string Styled;
		int InputLineVisibleLength;
		std::string RightBadges;
		int RightBadgesVisibleLength;
	};

	BuiltPrompt Build(int /*terminalWidth*/) const {
		// Collect segments
		std::vector<Segment> leftSegments;
		for (auto& provider : m_LeftProviders) {
			auto segment = provider();
			if (!segment.Text.empty()) {
				leftSegments.push_back(std::move(segment));
			}
		}

		std::vector<Segment> rightSegments;
		for (auto& provider : m_RightProviders) {
			auto segment = provider();
			if (!segment.Text.empty()) {
				rightSegments.push_back(std::move(segment));
			}
		}

		// Build left side
		std::string leftStyled;
		int leftVisibleLength = 0;

		for (size_t i = 0; i < leftSegments.size(); i++) {
			auto& segment = leftSegments[i];
			std::string content;
			if (!segment.Icon.empty()) {
				content = segment.Icon + " " + segment.Text;
			} else {
				content = segment.Text;
			}

			if (segment.BackgroundColor >= 0) {
				// Rounded leading cap on first segment
				if (i == 0) {
					leftStyled += "\033[38;5;" + std::to_string(segment.BackgroundColor) + "m";
					leftStyled += "\xee\x82\xb6"; //
					leftStyled += "\033[0m";
					leftVisibleLength += 1;
				}

				leftStyled += "\033[38;5;" + std::to_string(segment.ForegroundColor)
					+ ";48;5;" + std::to_string(segment.BackgroundColor) + "m";
				leftStyled += " " + content + " ";
				leftStyled += "\033[0m";
				leftVisibleLength += ColumnWidth(content) + 2;

				// Separator or trailing cap
				if (i + 1 < leftSegments.size() && leftSegments[i + 1].BackgroundColor >= 0) {
					leftStyled += "\033[38;5;" + std::to_string(segment.BackgroundColor)
						+ ";48;5;" + std::to_string(leftSegments[i + 1].BackgroundColor) + "m";
					leftStyled += "\xee\x82\xb0"; //
					leftStyled += "\033[0m";
					leftVisibleLength += 1;
				} else {
					// Rounded trailing cap on last bg segment
					leftStyled += "\033[0m\033[38;5;" + std::to_string(segment.BackgroundColor) + ";49m";
					leftStyled += "\xee\x82\xb4"; //
					leftStyled += "\033[0m";
					leftVisibleLength += 1;
				}
			} else {
				leftStyled += "\033[38;5;" + std::to_string(segment.ForegroundColor) + "m";
				leftStyled += content;
				leftStyled += "\033[0m";
				leftVisibleLength += ColumnWidth(content);

				if (i + 1 < leftSegments.size()) {
					leftStyled += " ";
					leftVisibleLength += 1;
				}
			}
		}

		// Build right side
		std::string rightStyled;
		int rightVisibleLength = 0;

		for (size_t i = 0; i < rightSegments.size(); i++) {
			auto& segment = rightSegments[i];
			std::string content;
			if (!segment.Icon.empty()) {
				content = segment.Icon + " " + segment.Text;
			} else {
				content = segment.Text;
			}

			if (segment.BackgroundColor >= 0) {
				bool isFirst = (i == 0) || (rightSegments[i - 1].BackgroundColor < 0);
				bool isLast = (i + 1 >= rightSegments.size()) || (rightSegments[i + 1].BackgroundColor < 0);
				bool nextHasBg = !isLast && (i + 1 < rightSegments.size()) && rightSegments[i + 1].BackgroundColor >= 0;

				// Rounded left cap on first bg segment
				if (isFirst) {
					rightStyled += "\033[38;5;" + std::to_string(segment.BackgroundColor) + "m";
					rightStyled += "\xee\x82\xb6"; //
					rightStyled += "\033[0m";
					rightVisibleLength += 1;
				}

				// Content
				rightStyled += "\033[38;5;" + std::to_string(segment.ForegroundColor)
					+ ";48;5;" + std::to_string(segment.BackgroundColor) + "m";
				rightStyled += " " + content + " ";
				rightStyled += "\033[0m";
				rightVisibleLength += ColumnWidth(content) + 2;

				// Powerline separator or rounded right cap
				if (nextHasBg) {
					rightStyled += "\033[38;5;" + std::to_string(segment.BackgroundColor)
						+ ";48;5;" + std::to_string(rightSegments[i + 1].BackgroundColor) + "m";
					rightStyled += "\xee\x82\xb0"; //
					rightStyled += "\033[0m";
					rightVisibleLength += 1;
				} else {
					rightStyled += "\033[0m\033[38;5;" + std::to_string(segment.BackgroundColor) + ";49m";
					rightStyled += "\xee\x82\xb4"; //
					rightStyled += "\033[0m";
					rightVisibleLength += 1;
				}
			} else {
				if (i > 0) {
					rightStyled += " ";
					rightVisibleLength += 1;
				}

				rightStyled += "\033[38;5;" + std::to_string(segment.ForegroundColor) + "m";
				rightStyled += content;
				rightStyled += "\033[0m";
				rightVisibleLength += ColumnWidth(content);
			}
		}

		// Single line: left badges + prompt char
		std::string promptChar = "\033[1;38;5;" + std::to_string(m_PromptColor)
			+ "m" + m_PromptChar + "\033[0m ";

		std::string full = leftStyled + " " + promptChar;
		int inputVisibleLength = leftVisibleLength + 1 + ColumnWidth(m_PromptChar) + 1;

		return {full, inputVisibleLength, rightStyled, rightVisibleLength};
	}

private:
	std::vector<SegmentProvider> m_LeftProviders;
	std::vector<SegmentProvider> m_RightProviders;
	std::string m_PromptChar = "\xe2\x9d\xaf"; // ❯
	int m_PromptColor = 76; // green
};

}
