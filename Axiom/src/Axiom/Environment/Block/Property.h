#pragma once

#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Base class for all block properties. Each property
	 * has a name and a set of possible integer-indexed
	 * values that can be serialised to strings.
	 */
	class BlockProperty {
	public:
		explicit BlockProperty(std::string name) : m_Name(std::move(name)) {}
		virtual ~BlockProperty() = default;

		const std::string& Name() const { return m_Name; }
		virtual std::vector<std::string> PossibleValues() const = 0;
		virtual std::string ValueToString(int index) const = 0;
		virtual int ValueFromString(const std::string& text) const = 0;
		virtual int ValueCount() const = 0;

	private:
		std::string m_Name;
	};

	/**
	 * A block property that holds a boolean
	 * value — either "true" or "false".
	 */
	class BooleanProperty : public BlockProperty {
	public:
		explicit BooleanProperty(const std::string& name);

		std::vector<std::string> PossibleValues() const override;
		std::string ValueToString(int index) const override;
		int ValueFromString(const std::string& text) const override;
		int ValueCount() const override { return 2; }
	};

	/**
	 * A block property that holds an integer
	 * value in a contiguous range.
	 */
	class IntegerProperty : public BlockProperty {
	public:
		IntegerProperty(const std::string& name, int minimum, int maximum);

		std::vector<std::string> PossibleValues() const override;
		std::string ValueToString(int index) const override;
		int ValueFromString(const std::string& text) const override;
		int ValueCount() const override;

	private:
		int m_Min;
		int m_Max;
	};

	/**
	 * A block property backed by a fixed set
	 * of named string values (enum-like).
	 */
	class EnumProperty : public BlockProperty {
	public:
		EnumProperty(const std::string& name, std::vector<std::string> values);

		std::vector<std::string> PossibleValues() const override;
		std::string ValueToString(int index) const override;
		int ValueFromString(const std::string& text) const override;
		int ValueCount() const override;

	private:
		std::vector<std::string> m_Values;
	};

}
