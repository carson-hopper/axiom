#include "Property.h"

#include <algorithm>
#include <stdexcept>

namespace Axiom {

	// ── BooleanProperty ──────────────────────────────────────────────

	BooleanProperty::BooleanProperty(const std::string& name)
		: BlockProperty(name) {}

	std::vector<std::string> BooleanProperty::PossibleValues() const {
		return {"false", "true"};
	}

	std::string BooleanProperty::ValueToString(int index) const {
		return index != 0 ? "true" : "false";
	}

	int BooleanProperty::ValueFromString(const std::string& text) const {
		if (text == "true") return 1;
		if (text == "false") return 0;
		throw std::invalid_argument("BooleanProperty: invalid value '" + text + "'");
	}

	// ── IntegerProperty ──────────────────────────────────────────────

	IntegerProperty::IntegerProperty(const std::string& name, int minimum, int maximum)
		: BlockProperty(name)
		, m_Min(minimum)
		, m_Max(maximum) {}

	std::vector<std::string> IntegerProperty::PossibleValues() const {
		std::vector<std::string> result;
		result.reserve(static_cast<size_t>(m_Max - m_Min + 1));
		for (int i = m_Min; i <= m_Max; ++i) {
			result.push_back(std::to_string(i));
		}
		return result;
	}

	std::string IntegerProperty::ValueToString(int index) const {
		return std::to_string(m_Min + index);
	}

	int IntegerProperty::ValueFromString(const std::string& text) const {
		int value = std::stoi(text);
		if (value < m_Min || value > m_Max) {
			throw std::out_of_range("IntegerProperty: value " + text + " out of range");
		}
		return value - m_Min;
	}

	int IntegerProperty::ValueCount() const {
		return m_Max - m_Min + 1;
	}

	// ── EnumProperty ─────────────────────────────────────────────────

	EnumProperty::EnumProperty(const std::string& name, std::vector<std::string> values)
		: BlockProperty(name)
		, m_Values(std::move(values)) {}

	std::vector<std::string> EnumProperty::PossibleValues() const {
		return m_Values;
	}

	std::string EnumProperty::ValueToString(int index) const {
		return m_Values.at(static_cast<size_t>(index));
	}

	int EnumProperty::ValueFromString(const std::string& text) const {
		auto iterator = std::find(m_Values.begin(), m_Values.end(), text);
		if (iterator == m_Values.end()) {
			throw std::invalid_argument("EnumProperty: unknown value '" + text + "'");
		}
		return static_cast<int>(std::distance(m_Values.begin(), iterator));
	}

	int EnumProperty::ValueCount() const {
		return static_cast<int>(m_Values.size());
	}

}
