#pragma once

#include "Axiom/Utilities/Memory/Ref.h"

namespace Axiom {

	template<typename T>
	class WeakRef {
	public:
		WeakRef() = default;

		WeakRef(Ref<T> ref) {
			m_Instance = ref.Raw();
		}

		WeakRef(T* instance) {
			m_Instance = instance;
		}

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		bool IsValid() const { return m_Instance ? RefUtils::IsLive(m_Instance) : false; }
		operator bool() const { return IsValid(); }

		template<typename T2>
		WeakRef<T2> As() const {
			return WeakRef<T2>(dynamic_cast<T2*>(m_Instance));
		}
	private:
		T* m_Instance = nullptr;
	};

}