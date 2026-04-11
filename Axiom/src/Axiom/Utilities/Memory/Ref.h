#pragma once

#include <atomic>
#include <cstddef>
#include <type_traits>

namespace Axiom {

	/**
	 * Base class for every type managed by Ref<T>.
	 * Holds an atomic refcount and provides the inc /
	 * dec / get primitives that Ref<T> calls into.
	 *
	 * Intrusive refcounting (counter on the object,
	 * not in a separate control block) is what lets
	 * Axiom write Ref<T>(this) inside any method of
	 * a RefCounted-derived class to extend its own
	 * lifetime across async callbacks. See STYLE.md
	 * §4.2 for the rationale.
	 *
	 * Classes that want to be held by Ref<T> must
	 * publicly inherit from RefCounted — Ref<T>'s
	 * raw-pointer constructor enforces this at
	 * compile time via a static_assert on
	 * is_base_of_v<RefCounted, T>.
	 */
	class RefCounted {
	public:
		virtual ~RefCounted() = default;

		/**
		 * Relaxed is sufficient for increments. The
		 * caller already holds a live Ref to this
		 * object, which means the synchronization that
		 * made the object visible to them happened
		 * earlier — no new happens-before is needed.
		 */
		void IncRefCount() const {
			m_RefCount.fetch_add(1, std::memory_order_relaxed);
		}

		/**
		 * Returns the PRIOR value of the refcount. The
		 * caller uses `prior == 1` to decide whether it
		 * is the last owner and must run cleanup. We
		 * MUST NOT split the decrement and the zero
		 * check into two atomic operations — see
		 * `Ref<T>::DecRef` for the race that closes.
		 *
		 * `acq_rel` gives us release on the decrement
		 * (so every writer's prior writes are visible
		 * to whoever ends up deleting) and acquire for
		 * the winning thread (so it sees every other
		 * decrementer's writes before tearing down).
		 */
		uint32_t DecRefCount() const {
			return m_RefCount.fetch_sub(1, std::memory_order_acq_rel);
		}

		uint32_t GetRefCount() const { return m_RefCount.load(std::memory_order_relaxed); }
	private:
		mutable std::atomic<uint32_t> m_RefCount = 0;
	};

	namespace RefUtils {
		void AddToLiveReferences(void* instance);
		void RemoveFromLiveReferences(void* instance);
		bool IsLive(void*instance);
	}

	/**
	 * Intrusive shared-ownership smart pointer.
	 *
	 * Wraps a raw pointer to a RefCounted-derived
	 * object and manages the object's lifetime via
	 * inc/dec on its intrusive atomic refcount. When
	 * the count drops to zero, Ref<T>::DecRef deletes
	 * the object. Thread-safe for concurrent copies
	 * and moves of distinct Ref<T> instances; the
	 * refcount itself is a `std::atomic<uint32_t>`.
	 *
	 * Construction paths:
	 *   Ref<T>()                — null Ref
	 *   Ref<T>(nullptr)         — null Ref
	 *   Ref<T>(T*)              — adopt a raw pointer
	 *                             and increment the
	 *                             refcount (requires
	 *                             T derive from
	 *                             RefCounted)
	 *   Ref<T>::Create(args...) — preferred factory;
	 *                             allocates via new
	 *                             and wraps in a Ref
	 *
	 * The Ref<T>(T*) constructor is how the
	 * Ref<T>(this) idiom works — any method on a
	 * RefCounted-derived class can wrap `this` into
	 * a fresh Ref<T> to extend lifetime across an
	 * async callback. See STYLE.md §4.2 for the
	 * idiom and the requires-count-greater-than-zero
	 * precondition (never call from a destructor).
	 */
	template<typename T>
	class Ref {
	public:
		Ref()
			: m_Instance(nullptr) {}
		
		Ref(std::nullptr_t)
			: m_Instance(nullptr) {}

		Ref(T* instance)
			: m_Instance(instance) {
			static_assert(std::is_base_of_v<RefCounted, T>, "Class is not RefCounted!");

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

		/**
		 * Atomically drop one reference and, if we were
		 * the last owner, tear the object down.
		 *
		 * We check the PRIOR value returned by
		 * `DecRefCount` (a `fetch_sub` under the hood)
		 * instead of loading `GetRefCount()` separately.
		 * The two-operation form has a fatal race:
		 *
		 *   RefCount = 2
		 *     Thread A: --count  → 1
		 *     Thread B: --count  → 0
		 *     Thread A: load == 0, proceed
		 *     Thread B: load == 0, proceed
		 *     Thread A: delete   (free)
		 *     Thread B: delete   (double-free / UAF)
		 *
		 * With `fetch_sub` returning the prior value,
		 * exactly one thread sees `1` (the only thread
		 * that atomically brought the count 1 → 0) and
		 * is alone in running the teardown path. Every
		 * other decrementer sees a prior value > 1 and
		 * bails cleanly without ever touching the
		 * object again.
		 */
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