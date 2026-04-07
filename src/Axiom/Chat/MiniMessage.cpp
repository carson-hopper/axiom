#include "axpch.h"
#include "Axiom/Chat/MiniMessage.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	Ref<ChatComponent> MiniMessage::Parse(const std::string> message) {
		ParseState state;
		state.Input = message;
		state.Root = CreateRef<ChatComponent>();
		state.Root->Text = "";
		state.ComponentStack.push(state.Root);

		std::string currentText;

		while (state.Position < state.Input.size()) {
			if (state.Input[state.Position] == '<') {
				// Process any accumulated text first
				if (!currentText.empty()) {
					ProcessText(state, currentText);
					currentText.clear();
				}

				// Parse the tag
				Tag tag = ParseTag(state);
				ProcessTag(state, tag);
			} else {
				// Regular character
				currentText += state.Input[state.Position];
				state.Position++;
			}
		}

		// Process any remaining text
		if (!currentText.empty()) {
			ProcessText(state, currentText);
		}

		return state.Root;
	}

	Ref<ChatComponent> MiniMessage::ParseOrPlain(const std::string> message) {
		try {
			return Parse(message);
		} catch (...) {
			// Return plain text on error
			auto component = CreateRef<ChatComponent>();
			component->Text = message;
			return component;
		}
	}

	MiniMessage::Tag MiniMessage::ParseTag(ParseState& state) {
		Tag tag;
		tag.IsClosing = false;

		// Skip the opening '<'
		state.Position++;

		// Check for closing tag
		if (state.Position < state.Input.size() && state.Input[state.Position] == '/') {
			tag.IsClosing = true;
			state.Position++;
		}

		// Parse tag name
		tag.Name = ParseTagName(state);

		// Parse arguments (if any)
		if (state.Position < state.Input.size() && state.Input[state.Position] == ':') {
			tag.Arguments = ParseTagArguments(state);
		}

		// Skip the closing '>'
		if (state.Position < state.Input.size() && state.Input[state.Position] == '>') {
			state.Position++;
		}

		return tag;
	}

	std::string MiniMessage::ParseTagName(ParseState& state) {
		std::string name;
		while (state.Position < state.Input.size()) {
			char c = state.Input[state.Position];
			if (c == '>' || c == ':' || c == ' ')
				break;
			name += c;
			state.Position++;
		}
		return name;
	}

	std::vector<std::string> MiniMessage::ParseTagArguments(ParseState& state) {
		std::vector<std::string> args;

		// Skip ':'
		state.Position++;

		std::string currentArg;
		bool inQuotes = false;

		while (state.Position < state.Input.size()) {
			char c = state.Input[state.Position];

			if (c == '>' && !inQuotes) {
				break;
			}

			if (c == '"' || c == ''') {
				inQuotes = !inQuotes;
				state.Position++;
				continue;
			}

			if (c == ':' && !inQuotes) {
				if (!currentArg.empty()) {
					args.push_back(currentArg);
					currentArg.clear();
				}
				state.Position++;
				continue;
			}

			currentArg += c;
			state.Position++;
		}

		if (!currentArg.empty()) {
			args.push_back(currentArg);
		}

		return args;
	}

	void MiniMessage::ProcessTag(ParseState& state, const Tag& tag) {
		std::string lowerName;
		for (char c : tag.Name) {
			lowerName += std::tolower(c);
		}

		// Handle closing tags
		if (tag.IsClosing) {
			if (!state.ComponentStack.empty()) {
				state.ComponentStack.pop();
			}
			return;
		}

		// Create new component based on tag
		auto newComponent = CreateRef<ChatComponent>();

		// Color tags
		auto color = ParseColor(lowerName);
		if (color.has_value()) {
			newComponent->Color = color;
		} else if (lowerName.starts_with("#")) {
			// Hex color - convert to nearest standard color for now
			// TODO: Support true hex colors with adventure mod format
			newComponent->Color = ChatColor::White;
		}

		// Formatting tags
		if (lowerName == "bold" || lowerName == "b")
			newComponent->Bold = true;
		else if (lowerName == "italic" || lowerName == "i" || lowerName == "em")
			newComponent->Italic = true;
		else if (lowerName == "underlined" || lowerName == "u")
			newComponent->Underlined = true;
		else if (lowerName == "strikethrough" || lowerName == "st")
			newComponent->Strikethrough = true;
		else if (lowerName == "obfuscated" || lowerName == "obf")
			newComponent->Obfuscated = true;

		// Special tags
		else if (lowerName == "newline" || lowerName == "br") {
			newComponent->Text = "\n";
			if (!state.ComponentStack.empty()) {
				state.ComponentStack.top()->Extra.push_back(newComponent);
			}
			return;
		}
		else if (lowerName == "reset") {
			newComponent->Color = ChatColor::Reset;
		}

		// Click events
		else if (lowerName == "click" && !tag.Arguments.empty()) {
			auto action = ParseClickAction(tag.Arguments[0]);
			if (action.has_value() && tag.Arguments.size() >= 2) {
				newComponent->ClickEvent = ChatClickEvent(action.value(), tag.Arguments[1]);
			}
		}

		// Hover events
		else if (lowerName == "hover" && !tag.Arguments.empty()) {
			auto action = ParseHoverAction(tag.Arguments[0]);
			if (action.has_value() && tag.Arguments.size() >= 2) {
				auto hoverText = CreateRef<ChatComponent>();
				hoverText->Text = tag.Arguments[1];
				newComponent->HoverEvent = ChatHoverEvent(action.value(), hoverText);
			}
		}

		// Insertion
		else if (lowerName == "insertion" && !tag.Arguments.empty()) {
			newComponent->Insertion = tag.Arguments[0];
		}

		// Add to parent if we have one
		if (!state.ComponentStack.empty()) {
			state.ComponentStack.top()->Extra.push_back(newComponent);
			state.ComponentStack.push(newComponent);
		}
	}

	void MiniMessage::ProcessText(ParseState& state, const std::string> text) {
		if (state.ComponentStack.empty()) {
			// Add to root
			state.Root->Text += text;
		} else {
			// Add to current component
			auto& current = state.ComponentStack.top();
			if (current->Extra.empty()) {
				current->Text += text;
			} else {
				// Add as extra component
				auto textComponent = CreateRef<ChatComponent>();
				textComponent->Text = text;
				current->Extra.push_back(textComponent);
			}
		}
	}

	std::optional<ChatColor> MiniMessage::ParseColor(const std::string> name) {
		static const std::unordered_map<std::string, ChatColor> colorMap = {
			{"black", ChatColor::Black},
			{"dark_blue", ChatColor::DarkBlue},
			{"dark_green", ChatColor::DarkGreen},
			{"dark_aqua", ChatColor::DarkAqua},
			{"dark_cyan", ChatColor::DarkAqua},
			{"dark_red", ChatColor::DarkRed},
			{"dark_purple", ChatColor::DarkPurple},
			{"gold", ChatColor::Gold},
			{"yellow", ChatColor::Yellow},
			{"gray", ChatColor::Gray},
			{"grey", ChatColor::Gray},
			{"dark_gray", ChatColor::DarkGray},
			{"dark_grey", ChatColor::DarkGray},
			{"blue", ChatColor::Blue},
			{"green", ChatColor::Green},
			{"aqua", ChatColor::Aqua},
			{"cyan", ChatColor::Aqua},
			{"red", ChatColor::Red},
			{"light_purple", ChatColor::LightPurple},
			{"pink", ChatColor::LightPurple},
			{"white", ChatColor::White},
			{"reset", ChatColor::Reset}
		};

		auto it = colorMap.find(name);
		if (it != colorMap.end()) {
			return it->second;
		}
		return std::nullopt;
	}

	std::optional<ChatColor> MiniMessage::ParseHexColor(const std::string> hex) {
		// For now, return nullopt - true hex color support requires different approach
		return std::nullopt;
	}

	std::optional<ClickAction> MiniMessage::ParseClickAction(const std::string> name) {
		static const std::unordered_map<std::string, ClickAction> actionMap = {
			{"open_url", ClickAction::OpenUrl},
			{"open_file", ClickAction::OpenFile},
			{"run_command", ClickAction::RunCommand},
			{"suggest_command", ClickAction::SuggestCommand},
			{"change_page", ClickAction::ChangePage},
			{"copy_to_clipboard", ClickAction::CopyToClipboard}
		};

		auto it = actionMap.find(name);
		if (it != actionMap.end()) {
			return it->second;
		}
		return std::nullopt;
	}

	std::optional<HoverAction> MiniMessage::ParseHoverAction(const std::string> name) {
		static const std::unordered_map<std::string, HoverAction> actionMap = {
			{"show_text", HoverAction::ShowText},
			{"show_item", HoverAction::ShowItem},
			{"show_entity", HoverAction::ShowEntity}
		};

		auto it = actionMap.find(name);
		if (it != actionMap.end()) {
			return it->second;
		}
		return std::nullopt;
	}

}
