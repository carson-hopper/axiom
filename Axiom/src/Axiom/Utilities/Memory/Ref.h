#pragma once

#include "Axiom/Core/Assert.h"


#include <atomic>
#include <cstddef>
#include <type_traits>

namespace Axiom {

	class RefCounted {
	public:
		virtual ~RefCounted() = default;

		void IncRefCount() const { m_RefCount.fetch_add(1, std::memory_order_relaxed); }
		uint32_t DecRefCount() const { return m_RefCount.fetch_sub(1, std::memory_order_acq_rel); }
		uint32_t GetRefCount() const { return m_RefCount.load(std::memory_order_relaxed); }
	private:
		mutable std::atomic<uint32_t> m_RefCount = 0;
	};

	namespace RefUtils {
		void AddToLiveReferences(void* instance);
		void RemoveFromLiveReferences(void* instance);
		bool IsLive(void*instance);
	}

	template<typename T>
	class Ref {
	public:
		Ref()
			: m_Instance(nullptr) {}
		
		Ref(std::nullptr_t)
			: m_Instance(nullptr) {}

		Ref(T* instance)
			: m_Instance(instance) {
			static_assert(std::is_base_of_v<RefCounted, T>,
				"Ref<T>: T must derive from RefCounted");
			IncRef();
		}

		template<typename T2>
		Ref(const Ref<T2>& other)
			: m_Instance(static_cast<T*>(other.m_Instance)) {
			IncRef();
		}

		template<typename T2>
		Ref(Ref<T2>&& other) 
            : m_Instance(static_cast<T*>(other.m_Instance)) {
			other.m_Instance = nullptr;
		}

		~Ref() {
			DecRef();
		}

		Ref(const Ref<T>& other)
			: m_Instance(other.m_Instance) {
			IncRef();
		}

		Ref& operator=(std::nullptr_t) {
			DecRef();
			m_Instance = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& other) {
			if (this == &other) {
				return *this;
			}

			other.IncRef();
			DecRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(const Ref<T2>& other) {
			other.IncRef();
			DecRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(Ref<T2>&& other) {
			DecRef();

			m_Instance = other.m_Instance;
			other.m_Instance = nullptr;
			return *this;
		}

		operator bool() { return m_Instance != nullptr; }
		operator bool() const { return m_Instance != nullptr; }

		T* operator->() const { return m_Instance; }
		T& operator*() const { return *m_Instance; }
		T* Raw() const { return m_Instance; }

		void Reset(T* instance = nullptr){
			DecRef();
			m_Instance = instance;
		}

		template<typename T2>
		Ref<T2> As() const{
			return Ref<T2>(*this);
		}

		template<typename... Args>
		static Ref<T> Create(Args&&... args) {
            return Ref<T>(new T(std::forward<Args>(args)...));
		}

		bool operator==(const Ref<T>& other) const {
			return m_Instance == other.m_Instance;
		}

		bool operator!=(const Ref<T>& other) const {
			return !(*this == other);
		}

		bool EqualsObject(const Ref<T>& other) {
			if (!m_Instance || !other.m_Instance)
				return false;
			
			return *m_Instance == *other.m_Instance;
		}
	private:
		void IncRef() const {
			if (m_Instance) {
				m_Instance->IncRefCount();
				RefUtils::AddToLiveReferences(static_cast<void*>(m_Instance));
			}
		}

		void DecRef() const {
			if (!m_Instance) {
				return;
			}

			if (m_Instance->DecRefCount() != 1) {
				return;
			}

			RefUtils::RemoveFromLiveReferences(static_cast<void*>(m_Instance));
			delete m_Instance;
			m_Instance = nullptr;
		}

		template<class T2>
		friend class Ref;
		mutable T* m_Instance;
	};

}