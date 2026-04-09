#include "BlockInteraction.h"

namespace Axiom {

	void BlockInteraction::RegisterPlacement(int32_t blockId, PlacementCallback callback) {
		m_PlacementCallbacks[blockId] = std::move(callback);
	}

	void BlockInteraction::RegisterInteraction(int32_t blockId, InteractionCallback callback) {
		m_InteractionCallbacks[blockId] = std::move(callback);
	}

	void BlockInteraction::RegisterDestroy(int32_t blockId, DestroyCallback callback) {
		m_DestroyCallbacks[blockId] = std::move(callback);
	}

	int32_t BlockInteraction::GetStateForPlacement(int32_t blockId, int x, int y, int z, int face) const {
		auto iterator = m_PlacementCallbacks.find(blockId);
		if (iterator != m_PlacementCallbacks.end()) {
			return iterator->second(x, y, z, face);
		}
		return blockId;
	}

	bool BlockInteraction::OnInteract(int32_t blockId, int x, int y, int z, Entity& entity) const {
		auto iterator = m_InteractionCallbacks.find(blockId);
		if (iterator != m_InteractionCallbacks.end()) {
			return iterator->second(x, y, z, entity);
		}
		return false;
	}

	void BlockInteraction::OnDestroy(int32_t blockId, int x, int y, int z) const {
		auto iterator = m_DestroyCallbacks.find(blockId);
		if (iterator != m_DestroyCallbacks.end()) {
			iterator->second(x, y, z);
		}
	}

}
