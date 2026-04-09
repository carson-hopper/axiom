#pragma once

#include "Axiom/Core/Base.h"

#include <string>
#include <unordered_map>

namespace Axiom {

	/**
	 * Describes a single data component type
	 * that can be attached to an item stack
	 * (e.g. minecraft:damage, minecraft:lore).
	 */
	class DataComponentType {
	public:
		explicit DataComponentType(std::string name) : m_Name(std::move(name)) {}
		const std::string& Name() const { return m_Name; }

	private:
		std::string m_Name;
	};

	/**
	 * Global registry for the 1.21 item data
	 * component system. Call Bootstrap() once
	 * at startup to register built-in types.
	 */
	class DataComponentRegistry {
	public:
		static DataComponentRegistry& Instance();

		void Register(const std::string& name);
		bool IsRegistered(const std::string& name) const;

		/**
		 * Register all built-in
		 * component types.
		 */
		static void Bootstrap();

	private:
		std::unordered_map<std::string, Scope<DataComponentType>> m_Types;
		static bool s_Bootstrapped;
	};

}
