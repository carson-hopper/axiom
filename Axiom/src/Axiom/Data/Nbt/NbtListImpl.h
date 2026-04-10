#pragma once

#include "Axiom/Data/Nbt/NbtCompound.h"
#include "Axiom/Data/Nbt/NbtList.h"

namespace Axiom {

	inline Ref<NbtCompound> NbtList::GetCompound(const size_t index) const {
		const auto tag = Get(index);
		if (tag && tag->Type() == NbtTagType::Compound) {
			return tag.As<NbtCompound>();
		}
		return nullptr;
	}

	inline Ref<NbtList> NbtList::GetList(const size_t index) const {
		const auto tag = Get(index);
		if (tag && tag->Type() == NbtTagType::List) {
			return tag.As<NbtList>();
		}
		return nullptr;
	}

	inline std::string NbtList::GetString(const size_t index) const {
		const auto tag = Get(index);
		if (tag && tag->Type() == NbtTagType::String) {
			return static_cast<NbtString*>(tag.Raw())->Value();
		}
		return "";
	}

	inline int32_t NbtList::GetInt(const size_t index) const {
		const auto tag = Get(index);
		if (tag && tag->Type() == NbtTagType::Int) {
			return static_cast<NbtInt*>(tag.Raw())->Value();
		}
		return 0;
	}

	inline int64_t NbtList::GetLong(const size_t index) const {
		const auto tag = Get(index);
		if (tag && tag->Type() == NbtTagType::Long) {
			return static_cast<NbtLong*>(tag.Raw())->Value();
		}
		return 0;
	}

	inline float NbtList::GetFloat(const size_t index) const {
		const auto tag = Get(index);
		if (tag && tag->Type() == NbtTagType::Float) {
			return static_cast<NbtFloat*>(tag.Raw())->Value();
		}
		return 0.0F;
	}

	inline double NbtList::GetDouble(const size_t index) const {
		const auto tag = Get(index);
		if (tag && tag->Type() == NbtTagType::Double) {
			return static_cast<NbtDouble*>(tag.Raw())->Value();
		}
		return 0.0;
	}

}
