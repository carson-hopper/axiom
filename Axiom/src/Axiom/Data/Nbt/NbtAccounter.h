#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace Axiom {

	/**
	 * Memory and depth accounting guard for NBT parsing.
	 * Prevents stack overflow and memory exhaustion from
	 * malicious or malformed NBT payloads.
	 *
	 * Every `NbtTag::Read` override walks the payload
	 * through an `NbtAccounter` — bytes consumed flow
	 * through `AccountBytes`, and every recursion into
	 * a `NbtList` / `NbtCompound` holds a `DepthGuard`.
	 * Budget or depth exhaustion throws, so the parse
	 * aborts cleanly instead of exhausting memory or
	 * smashing the stack.
	 */
	class NbtAccounter {
	public:
		static constexpr int MAX_DEPTH = 512;
		static constexpr size_t DEFAULT_BUDGET = 2 * 1024 * 1024;

		explicit NbtAccounter(size_t maxBytes = DEFAULT_BUDGET);

		/**
		 * Push one level of nesting depth.
		 * Returns false if MAX_DEPTH would be
		 * exceeded by this push.
		 */
		bool PushDepth();

		/**
		 * Pop one level of nesting depth.
		 * Returns false if depth is
		 * already at zero.
		 */
		bool PopDepth();

		/**
		 * Account for the given number of bytes.
		 * Returns false if the cumulative total
		 * would exceed the byte budget. The check
		 * is written as `count > maxBytes - used`
		 * rather than `used + count > maxBytes`
		 * so a near-SIZE_MAX `count` can't wrap
		 * the sum around and silently pass.
		 */
		bool AccountBytes(size_t count);

		int CurrentDepth() const { return m_CurrentDepth; }
		size_t BytesAccounted() const { return m_BytesAccounted; }
		size_t ByteBudget() const { return m_MaxBytes; }

		/**
		 * Human-readable description of the
		 * last error that occurred.
		 */
		const std::string& LastError() const { return m_LastError; }

		/**
		 * Reset all counters to their
		 * initial state.
		 */
		void Reset();

		/**
		 * RAII depth-scope helper. Constructor
		 * calls `PushDepth` (throws on overflow);
		 * destructor calls `PopDepth`, guaranteeing
		 * the depth counter is unwound even if the
		 * enclosing read throws mid-parse.
		 */
		class DepthGuard {
		public:
			explicit DepthGuard(NbtAccounter& accounter);
			~DepthGuard();

			DepthGuard(const DepthGuard&) = delete;
			DepthGuard& operator=(const DepthGuard&) = delete;
			DepthGuard(DepthGuard&& other) noexcept
				: m_Accounter(other.m_Accounter) {
				other.m_Accounter = nullptr;
			}
			DepthGuard& operator=(DepthGuard&&) = delete;

		private:
			NbtAccounter* m_Accounter;
		};

	private:
		size_t m_MaxBytes;
		size_t m_BytesAccounted = 0;
		int m_CurrentDepth = 0;
		std::string m_LastError;
	};

}
