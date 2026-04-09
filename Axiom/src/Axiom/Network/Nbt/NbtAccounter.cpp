#include "NbtAccounter.h"

#include <format>

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
	if (m_BytesAccounted + count > m_MaxBytes) {
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

}
