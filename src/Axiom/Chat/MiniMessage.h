#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Chat/ChatComponent.h"

#include <string>
#include <vector>
#include <stack>

namespace Axiom {

/**
 * Parser for MiniMessage format text.
 *
 * MiniMessage is a user-friendly format for creating rich chat messages using tags.
 * It was popularized by PaperMC and is widely used in the Minecraft community.
 *
 * Supported tags:
 * - Colors: <red>, <green>, <blue>, <#ff0000>, etc.
 * - Formatting: <bold>, <italic>, <underlined>, <strikethrough>, <obfuscated>
 * - Click events: <click:run_command:/help>, <click:open_url:https://...>
 * - Hover events: <hover:show_text:Hello>, <hover:show_item:diamond>
 * - Special: <reset>, <newline>, <keybind:key.jump>
 *
 * Example:
 * @code
 * auto message = MiniMessage::Parse(
 *     "<green>Hello <bold>player</bold>!</green> "
 *     "<click:run_command:/help><yellow>[Click for help]</yellow></click>"
 * );
 *
 * sender.SendMessage(message);
 * @endcode
 *
 * @see https://docs.papermc.io/adventure/minimessage/
 */
class MiniMessage {
public:
	/**
	 * Parse a MiniMessage format string into a ChatComponent.
	 *
	 * @param message The MiniMessage format string.
	 * @return A ChatComponent representing the parsed message.
	 */
	static Ref<ChatComponent> Parse(const std::string> message);

	/**
	 * Parse with a fallback. If parsing fails, returns a component with the plain text.
	 *
	 * @param message The MiniMessage format string.
	 * @return A ChatComponent (never null).
	 */
	static Ref<ChatComponent> ParseOrPlain(const std::string> message);

private:
	// Tag types
	enum class TagType {
		Color,
		Formatting,
		Click,
		Hover,
		Reset,
		Newline,
		Keybind,
		Translatable,
		Insertion,
		Font,
		Gradient,
		Rainbow,
		Transition
	};

	struct Tag {
		std::string Name;
		TagType Type;
		bool IsClosing;
		std::vector<std::string> Arguments;
	};

	// Parser state
	struct ParseState {
		std::string Input;
		size_t Position = 0;
		Ref<ChatComponent> Root;
		std::stack<Ref<ChatComponent>> ComponentStack;
	};

private:
	static Tag ParseTag(ParseState& state);
	static std::string ParseTagName(ParseState& state);
	static std::vector<std::string> ParseTagArguments(ParseState& state);
	static void ProcessTag(ParseState& state, const Tag& tag);
	static void ProcessText(ParseState& state, const std::string> text);

	// Color parsing
	static std::optional<ChatColor> ParseColor(const std::string> name);
	static std::optional<ChatColor> ParseHexColor(const std::string> hex);

	// Click action parsing
	static std::optional<ClickAction> ParseClickAction(const std::string> name);

	// Hover action parsing
	static std::optional<HoverAction> ParseHoverAction(const std::string> name);
};

}
