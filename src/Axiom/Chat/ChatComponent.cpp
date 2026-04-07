#include "axpch.h"
#include "Axiom/Chat/ChatComponent.h"

#include <nlohmann/json.hpp>

namespace Axiom {

	std::string ChatColorToString(const ChatColor color) {
		switch (color) {
			case ChatColor::Black: return "black";
			case ChatColor::DarkBlue: return "dark_blue";
			case ChatColor::DarkGreen: return "dark_green";
			case ChatColor::DarkAqua: return "dark_aqua";
			case ChatColor::DarkRed: return "dark_red";
			case ChatColor::DarkPurple: return "dark_purple";
			case ChatColor::Gold: return "gold";
			case ChatColor::Gray: return "gray";
			case ChatColor::DarkGray: return "dark_gray";
			case ChatColor::Blue: return "blue";
			case ChatColor::Green: return "green";
			case ChatColor::Aqua: return "aqua";
			case ChatColor::Red: return "red";
			case ChatColor::LightPurple: return "light_purple";
			case ChatColor::Yellow: return "yellow";
			case ChatColor::White: return "white";
			case ChatColor::Reset: return "reset";
		}
		return "white";
	}

	std::string ClickActionToString(const ClickAction action) {
		switch (action) {
			case ClickAction::OpenUrl: return "open_url";
			case ClickAction::OpenFile: return "open_file";
			case ClickAction::RunCommand: return "run_command";
			case ClickAction::SuggestCommand: return "suggest_command";
			case ClickAction::ChangePage: return "change_page";
			case ClickAction::CopyToClipboard: return "copy_to_clipboard";
		}
		return "";
	}

	std::string HoverActionToString(const HoverAction action) {
		switch (action) {
			case HoverAction::ShowText: return "show_text";
			case HoverAction::ShowItem: return "show_item";
			case HoverAction::ShowEntity: return "show_entity";
		}
		return "";
	}

	// ========== Builder Implementation ==========

	ChatComponent::Builder& ChatComponent::Builder::Text(const std::string& text) {
		m_Current->Text = text;
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Text(const std::string& text, ChatColor color) {
		m_Current->Text = text;
		m_Current->Color = color;
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Color(ChatColor color) {
		m_Current->Color = color;
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Bold(bool bold) {
		m_Current->Bold = bold;
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Italic(bool italic) {
		m_Current->Italic = italic;
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Underlined(bool underlined) {
		m_Current->Underlined = underlined;
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Strikethrough(bool strikethrough) {
		m_Current->Strikethrough = strikethrough;
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Obfuscated(bool obfuscated) {
		m_Current->Obfuscated = obfuscated;
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::ClickEvent(const ClickAction action, const std::string& value) {
		m_Current->ClickEvent = ChatClickEvent(action, value);
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::HoverEvent(const HoverAction action, const Ref<ChatComponent> &value) {
		m_Current->HoverEvent = Axiom::ChatHoverEvent(action, value);
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::HoverText(const std::string& text) {
		auto textComponent = std::make_shared<ChatComponent>();
		textComponent->Text = text;
		m_Current->HoverEvent = Axiom::ChatHoverEvent(HoverAction::ShowText, textComponent);
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Extra(const Ref<ChatComponent> &component) {
		m_Component->Extra.push_back(component);
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Append(const std::string& text) {
		auto component = std::make_shared<ChatComponent>();
		component->Text = text;
		m_Component->Extra.push_back(component);
		return *this;
	}

	ChatComponent::Builder& ChatComponent::Builder::Append(const std::string& text, ChatColor color) {
		auto component = std::make_shared<ChatComponent>();
		component->Text = text;
		component->Color = color;
		m_Component->Extra.push_back(component);
		return *this;
	}

	std::shared_ptr<ChatComponent> ChatComponent::Builder::Build() {
		return m_Component;
	}

	// ========== Serialization ==========

	std::string ChatComponent::ToJson() const {
		nlohmann::json json;

		json["text"] = Text;

		if (Color.has_value()) {
			json["color"] = ChatColorToString(Color.value());
		}
		if (Bold.has_value()) {
			json["bold"] = Bold.value();
		}
		if (Italic.has_value()) {
			json["italic"] = Italic.value();
		}
		if (Underlined.has_value()) {
			json["underlined"] = Underlined.value();
		}
		if (Strikethrough.has_value()) {
			json["strikethrough"] = Strikethrough.value();
		}
		if (Obfuscated.has_value()) {
			json["obfuscated"] = Obfuscated.value();
		}

		if (ClickEvent.has_value()) {
			json["clickEvent"]["action"] = ClickActionToString(ClickEvent->Action);
			json["clickEvent"]["value"] = ClickEvent->Value;
		}

		if (HoverEvent.has_value()) {
			json["hoverEvent"]["action"] = HoverActionToString(HoverEvent->Action);
			json["hoverEvent"]["contents"] = nlohmann::json::parse(HoverEvent->Value->ToJson());
		}

		if (Insertion.has_value()) {
			json["insertion"] = Insertion.value();
		}

		if (!Extra.empty()) {
			json["extra"] = nlohmann::json::array();
			for (const auto& extra : Extra) {
				json["extra"].push_back(nlohmann::json::parse(extra->ToJson()));
			}
		}

		return json.dump();
	}

	std::string ChatComponent::ToLegacyString() const {
		std::string result;

		// Add color code
		if (Color.has_value()) {
			// Minecraft uses § for color codes
			// Map colors to legacy codes
			static const char* colorCodes[] = {
				"§0", "§1", "§2", "§3", "§4", "§5", "§6", "§7",
				"§8", "§9", "§a", "§b", "§c", "§d", "§e", "§f", "§r"
			};
			int index = static_cast<int>(Color.value());
			if (index >= 0 && index < 17) {
				result += colorCodes[index];
			}
		}

		// Add formatting codes
		if (Bold.value_or(false)) result += "§l";
		if (Strikethrough.value_or(false)) result += "§m";
		if (Underlined.value_or(false)) result += "§n";
		if (Italic.value_or(false)) result += "§o";
		if (Obfuscated.value_or(false)) result += "§k";

		result += Text;

		// Append extra components
		for (const auto& extra : Extra) {
			result += extra->ToLegacyString();
		}

		return result;
	}

	std::string ChatComponent::GetPlainText() const {
		std::string result = Text;
		for (const auto& extra : Extra) {
			result += extra->GetPlainText();
		}
		return result;
	}

}
