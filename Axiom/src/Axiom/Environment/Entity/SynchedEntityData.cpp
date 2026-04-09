#include "axpch.h"
#include "Axiom/Environment/Entity/SynchedEntityData.h"

namespace Axiom {

	// ----- Define ---------------------------------------------------

	void SynchedEntityData::DefineBoolean(const int index, const bool defaultValue) {
		m_Entries[index] = {defaultValue, false};
	}

	void SynchedEntityData::DefineInt(const int index, const int32_t defaultValue) {
		m_Entries[index] = {defaultValue, false};
	}

	void SynchedEntityData::DefineFloat(const int index, const float defaultValue) {
		m_Entries[index] = {defaultValue, false};
	}

	void SynchedEntityData::DefineString(const int index, const std::string& defaultValue) {
		m_Entries[index] = {defaultValue, false};
	}

	// ----- Get ------------------------------------------------------

	bool SynchedEntityData::GetBoolean(const int index) const {
		auto iterator = m_Entries.find(index);
		if (iterator != m_Entries.end()) {
			return std::get<bool>(iterator->second.Value);
		}
		return false;
	}

	int32_t SynchedEntityData::GetInt(const int index) const {
		auto iterator = m_Entries.find(index);
		if (iterator != m_Entries.end()) {
			return std::get<int32_t>(iterator->second.Value);
		}
		return 0;
	}

	float SynchedEntityData::GetFloat(const int index) const {
		auto iterator = m_Entries.find(index);
		if (iterator != m_Entries.end()) {
			return std::get<float>(iterator->second.Value);
		}
		return 0.0f;
	}

	std::string SynchedEntityData::GetString(const int index) const {
		auto iterator = m_Entries.find(index);
		if (iterator != m_Entries.end()) {
			return std::get<std::string>(iterator->second.Value);
		}
		return "";
	}

	// ----- Set ------------------------------------------------------

	void SynchedEntityData::SetBoolean(const int index, const bool value) {
		auto iterator = m_Entries.find(index);
		if (iterator != m_Entries.end()) {
			iterator->second.Value = value;
			iterator->second.Dirty = true;
		}
	}

	void SynchedEntityData::SetInt(const int index, const int32_t value) {
		auto iterator = m_Entries.find(index);
		if (iterator != m_Entries.end()) {
			iterator->second.Value = value;
			iterator->second.Dirty = true;
		}
	}

	void SynchedEntityData::SetFloat(const int index, const float value) {
		auto iterator = m_Entries.find(index);
		if (iterator != m_Entries.end()) {
			iterator->second.Value = value;
			iterator->second.Dirty = true;
		}
	}

	void SynchedEntityData::SetString(const int index, const std::string& value) {
		auto iterator = m_Entries.find(index);
		if (iterator != m_Entries.end()) {
			iterator->second.Value = value;
			iterator->second.Dirty = true;
		}
	}

	// ----- Dirty tracking -------------------------------------------

	bool SynchedEntityData::IsDirty() const {
		for (const auto& [index, entry] : m_Entries) {
			if (entry.Dirty) return true;
		}
		return false;
	}

	std::vector<int> SynchedEntityData::GetDirtyIndices() const {
		std::vector<int> dirty;
		for (const auto& [index, entry] : m_Entries) {
			if (entry.Dirty) {
				dirty.push_back(index);
			}
		}
		return dirty;
	}

	void SynchedEntityData::ClearDirty() {
		for (auto& [index, entry] : m_Entries) {
			entry.Dirty = false;
		}
	}

}
