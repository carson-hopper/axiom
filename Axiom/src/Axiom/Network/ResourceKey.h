#pragma once

#include "Axiom/Network/ResourceLocation.h"

namespace Axiom {

	/**
	 * Typed resource key that identifies a
	 * specific entry in a specific
	 * registry by its
	 * location.
	 */
	template<typename T>
	class ResourceKey {
	public:
		ResourceKey(ResourceLocation registry, ResourceLocation location)
			: m_Registry(std::move(registry))
			, m_Location(std::move(location)) {}

		const ResourceLocation& Registry() const { return m_Registry; }
		const ResourceLocation& Location() const { return m_Location; }

		bool operator==(const ResourceKey&) const = default;

	private:
		ResourceLocation m_Registry;
		ResourceLocation m_Location;
	};

}
