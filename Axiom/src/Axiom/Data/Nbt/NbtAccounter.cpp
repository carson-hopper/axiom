#include "NbtAccounter.h"

#include <format>
#include <stdexcept>

namespace Axiom {

NbtAccounter::NbtAccounter(size_t maxBytes)
	: m_MaxBytes(maxBytes) {}

bool NbtAccounter::PushDepth() {
	if (m_CurrentDepth >= MAX_DEPTH) {
		m_LastError = std::format(
			"NBT depth limit exceeded: {} >= {}",
			m_CurrentDepth, MAX_DEPTH
		);
		return false;
	}
	++m_CurrentDepth;
	return true;
}

bool NbtAccounter::PopDepth() {
	if (m_CurrentDepth <= 0) {
		m_LastError = "Cannot pop NBT depth below zero";
		return false;
	}
	--m_CurrentDepth;
	return true;
}

bool NbtAccounter::AccountBytes(size_t count) {
	// Written as `count > max - used` not `used + count > max` so a
	// near-SIZE_MAX count can't wrap the sum around and silently pass.
	if (count > m_MaxBytes - m_BytesAccounted) {
		m_LastError = std::format(
			"NBT byte budget exceeded: {} + {} > {}",
			m_BytesAccounted, count, m_MaxBytes
		);
		return false;
	}
	m_BytesAccounted += count;
	return true;
}

void NbtAccounter::Reset() {
	m_BytesAccounted = 0;
	m_CurrentDepth = 0;
	m_LastError.clear();
}

NbtAccounter::DepthGuard::DepthGuard(NbtAccounter& accounter)
	: m_Accounter(&accounter) {
	if (!m_Accounter->PushDepth()) {
		throw std::runtime_error(m_Accounter->LastError());
	}
}

NbtAccounter::DepthGuard::~DepthGuard() {
	if (m_Accounter) {
		m_Accounter->PopDepth();
	}
}

}
