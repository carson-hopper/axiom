#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace Axiom {

	class NbtAccounter {
	public:
		static constexpr int MAX_DEPTH = 512;
		static constexpr size_t DEFAULT_BUDGET = 2 * 1024 * 1024;

		explicit NbtAccounter(size_t maxBytes = DEFAULT_BUDGET);

		bool PushDepth();
		bool PopDepth();
		bool AccountBytes(size_t count);

		int CurrentDepth() const { return m_CurrentDepth; }
		size_t BytesAccounted() const { return m_BytesAccounted; }
		size_t ByteBudget() const { return m_MaxBytes; }

		const std::string& LastError() const { return m_LastError; }

		void Reset();

		// RAII: PushDepth on construct (throws on overflow), PopDepth
		// on destruct — guarantees unwind even if the enclosing read
		// throws mid-parse.
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
