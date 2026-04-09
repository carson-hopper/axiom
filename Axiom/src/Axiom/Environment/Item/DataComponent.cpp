#include "DataComponent.h"

namespace Axiom {

	bool DataComponentRegistry::s_Bootstrapped = false;

	DataComponentRegistry& DataComponentRegistry::Instance() {
		static DataComponentRegistry instance;
		return instance;
	}

	void DataComponentRegistry::Register(const std::string& name) {
		m_Types[name] = CreateScope<DataComponentType>(name);
	}

	bool DataComponentRegistry::IsRegistered(const std::string& name) const {
		return m_Types.contains(name);
	}

	void DataComponentRegistry::Bootstrap() {
		if (s_Bootstrapped) return;
		s_Bootstrapped = true;

		auto& registry = Instance();
		registry.Register("minecraft:damage");
		registry.Register("minecraft:max_stack_size");
		registry.Register("minecraft:custom_name");
		registry.Register("minecraft:enchantments");
		registry.Register("minecraft:lore");
		registry.Register("minecraft:unbreakable");
		registry.Register("minecraft:repair_cost");
	}

}
