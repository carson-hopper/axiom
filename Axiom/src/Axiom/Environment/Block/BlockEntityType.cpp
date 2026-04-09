#include "BlockEntityType.h"

#include "Axiom/Core/Log.h"

namespace Axiom {

	void BlockEntityType::Register(BlockEntityType type) {
		auto& registry = Registry();
		const std::string identifier = type.Identifier();

		if (registry.contains(identifier)) {
			AX_CORE_WARN("BlockEntityType: overwriting existing type '{}'", identifier);
		}

		AX_CORE_TRACE("BlockEntityType: registered '{}'", identifier);
		registry.insert_or_assign(identifier, std::move(type));
	}

	const BlockEntityType* BlockEntityType::Get(const std::string& identifier) {
		auto& registry = Registry();
		auto iterator = registry.find(identifier);
		if (iterator != registry.end()) {
			return &iterator->second;
		}
		return nullptr;
	}

	const std::unordered_map<std::string, BlockEntityType>& BlockEntityType::All() {
		return Registry();
	}

	std::unordered_map<std::string, BlockEntityType>& BlockEntityType::Registry() {
		static std::unordered_map<std::string, BlockEntityType> registry;
		return registry;
	}

}
