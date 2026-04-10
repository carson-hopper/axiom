#pragma once

#include "Axiom/Core/Base.h"

#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace Axiom {

	/**
	 * Minecraft chat colors for styling text.
	 */
	enum class ChatColor {
		Black,
		DarkBlue,
		DarkGreen,
		DarkAqua,
		DarkRed,
		DarkPurple,
		Gold,
		Gray,
		DarkGray,
		Blue,
		Green,
		Aqua,
		Red,
		LightPurple,
		Yellow,
		White,
		Reset
	};

	std::string ChatColorToString(ChatColor color);

	/**
	 * Click action types for chat components.
	 */
	enum class ClickAction {
		OpenUrl,
		OpenFile,
		RunCommand,
		SuggestCommand,
		ChangePage,
		CopyToClipboard
	};

	std::string ClickActionToString(ClickAction action);

	/**
	 * Hover action types for chat components.
	 */
	enum class HoverAction {
		ShowText,
		ShowItem,
		ShowEntity
	};

	std::string HoverActionToString(HoverAction action);

	/**
	 * Represents a click event in a chat component.
	 */
	struct ChatClickEvent {
		ClickAction Action;
		std::string Value;

		ChatClickEvent(ClickAction action, std::string value)
			: Action(action), Value(std::move(value)) {}
	};

	/**
	 * Represents a hover event in a chat component.
	 */
	struct ChatHoverEvent {
		HoverAction Action;
		Ref<class ChatComponent> Value;

		ChatHoverEvent(HoverAction action, Ref<class ChatComponent> value)
			: Action(action), Value(std::move(value)) {}
	};

	/**
	 * Minecraft-style chat component for rich text messages.
	 *
	 * Supports colors, styling, click events, hover events, and nested components.
	 *
	 * Example:
	 * @code
	 * auto message = ChatComponent::Builder()
	 *     .Text("Hello ")
	 *     .Color(ChatColor::Green)
	 *     .Text("player")
	 *     .Bold()
	 *     .ClickEvent(ClickAction::RunCommand, "/help")
	 *     .HoverEvent(HoverAction::ShowText, ChatComponent::Builder().Text("Click for help").Build())
	 *     .Build();
	 *
	 * sender.SendMessage(message);
	 * @endcode
	 */
	class ChatComponent : public virtual RefCounted {
	public:
		/**
		 * Builder for constructing ChatComponents fluently.
		 */
		class Builder {
		public:
			Builder& Text(const std::string& text);
			Builder& Text(const std::string& text, ChatColor color);

			// Styling
			Builder& Color(ChatColor color);
			Builder& Bold(bool bold = true);
			Builder& Italic(bool italic = true);
			Builder& Underlined(bool underlined = true);
			Builder& Strikethrough(bool strikethrough = true);
			Builder& Obfuscated(bool obfuscated = true);

			// Events
			Builder& ClickEvent(ClickAction action, const std::string& value);
			Builder& HoverEvent(HoverAction action, const Ref<ChatComponent> &value);
			Builder& HoverText(const std::string& text);

		// Append extra components
		Builder& Extra(const Ref<ChatComponent> &component);
		Builder& Append(const std::string& text);
		Builder& Append(const std::string& text, ChatColor color);

		/**
		 * Add a space character.
		 * Equivalent to Append(" ")
		 */
		Builder& Space();

		/**
		 * Add multiple spaces.
		 * @param count Number of spaces to add (default: 1)
		 */
		Builder& Space(int count);

		/**
		 * Add a newline character.
		 * Starts a new line in the chat.
		 */
		Builder& NewLine();

		/**
		 * Add multiple newlines.
		 * @param count Number of newlines to add (default: 1)
		 */
		Builder& NewLine(int count);

		Ref<ChatComponent> Build();

		private:
			Ref<ChatComponent> m_Component = CreateRef<ChatComponent>();
			Ref<ChatComponent> m_Current = m_Component;
		};

		// Static helper to start building
		static Builder Create() { return Builder(); }

		// Serialize to JSON for Minecraft protocol
		std::string ToJson() const;

		// Serialize to legacy color codes (§ format)
		std::string ToLegacyString() const;

		// Get plain text without formatting
		std::string GetPlainText() const;

	public:
		std::string Text;
		std::optional<ChatColor> Color;
		std::optional<bool> Bold;
		std::optional<bool> Italic;
		std::optional<bool> Underlined;
		std::optional<bool> Strikethrough;
		std::optional<bool> Obfuscated;
		std::optional<ChatClickEvent> ClickEvent;
		std::optional<ChatHoverEvent> HoverEvent;
		std::vector<Ref<ChatComponent>> Extra;

		// Insertion text (for shift-click)
		std::optional<std::string> Insertion;
	};

} // namespace Axiom
