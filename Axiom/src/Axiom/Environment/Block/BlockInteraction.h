#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace Axiom {

	class Entity;

	/**
	 * Callback system for block placement,
	 * interaction, and destruction events.
	 */
	class BlockInteraction {
	public:
		using PlacementCallback = std::function<int32_t(int x, int y, int z, int face)>;
		using InteractionCallback = std::function<bool(int x, int y, int z, Entity& entity)>;
		using DestroyCallback = std::function<void(int x, int y, int z)>;

		void RegisterPlacement(int32_t blockId, PlacementCallback callback);
		void RegisterInteraction(int32_t blockId, InteractionCallback callback);
		void RegisterDestroy(int32_t blockId, DestroyCallback callback);

		int32_t GetStateForPlacement(int32_t blockId, int x, int y, int z, int face) const;
		bool OnInteract(int32_t blockId, int x, int y, int z, Entity& entity) const;
		void OnDestroy(int32_t blockId, int x, int y, int z) const;

	private:
		std::unordered_map<int32_t, PlacementCallback> m_PlacementCallbacks;
		std::unordered_map<int32_t, InteractionCallback> m_InteractionCallbacks;
		std::unordered_map<int32_t, DestroyCallback> m_DestroyCallbacks;
	};

}
