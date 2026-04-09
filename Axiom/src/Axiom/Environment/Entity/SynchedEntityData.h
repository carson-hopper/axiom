#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Axiom {

	/**
	 * Container for entity data values that
	 * require client synchronization
	 * (health, pose, flags,
	 * custom name).
	 */
	class SynchedEntityData {
	public:
		/** Define a data item by index and type. */
		void DefineBoolean(int index, bool defaultValue);
		void DefineInt(int index, int32_t defaultValue);
		void DefineFloat(int index, float defaultValue);
		void DefineString(int index, const std::string& defaultValue);

		/** Get data values. */
		bool GetBoolean(int index) const;
		int32_t GetInt(int index) const;
		float GetFloat(int index) const;
		std::string GetString(int index) const;

		/** Set data values (marks as dirty for sync). */
		void SetBoolean(int index, bool value);
		void SetInt(int index, int32_t value);
		void SetFloat(int index, float value);
		void SetString(int index, const std::string& value);

		/** Check if any values changed since last sync. */
		bool IsDirty() const;

		/** Get indices of dirty values. */
		std::vector<int> GetDirtyIndices() const;

		/** Mark all values as clean after syncing. */
		void ClearDirty();

	private:
		struct DataEntry {
			std::variant<bool, int32_t, float, std::string> Value;
			bool Dirty = false;
		};

		std::unordered_map<int, DataEntry> m_Entries;
	};

}
