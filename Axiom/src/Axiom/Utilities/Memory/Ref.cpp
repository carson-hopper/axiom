#include "axpch.h"

#include <unordered_set>

namespace Axiom {

	static std::unordered_set<void*> s_LiveReferences;
	static std::mutex s_LiveReferenceMutex;

	namespace RefUtils {

		void AddToLiveReferences(void* instance) {
			std::scoped_lock lock(s_LiveReferenceMutex);
			AX_CORE_ASSERT(instance);
			s_LiveReferences.insert(instance);
		}

		void RemoveFromLiveReferences(void* instance) {
			std::scoped_lock lock(s_LiveReferenceMutex);
			s_LiveReferences.erase(instance);
		}

		bool IsLive(void* const instance) {
			return s_LiveReferences.contains(instance);
		}
	}


}