#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace Axiom {

	/**
	 * Memory and depth accounting guard for NBT parsing.
	 * Prevents stack overflow and memory exhaustion from
	 * malicious or malformed NBT payloads.
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
		 * would exceed the byte budget.
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

	private:
		size_t m_MaxBytes;
		size_t m_BytesAccounted = 0;
		int m_CurrentDepth = 0;
		std::string m_LastError;
	};

}
