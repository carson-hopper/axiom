#include "TagLoader.h"

#include "Axiom/Core/Log.h"

#include <nlohmann/json.hpp>

#include <fstream>

namespace Axiom {

	const std::vector<std::string> TagLoader::s_EmptyTag;

	void TagLoader::Load(const std::filesystem::path& tagDirectory) {
		m_TagDirectory = tagDirectory;

		AX_CORE_INFO("TagLoader: loading tags from {}", tagDirectory.string());

		if (!std::filesystem::exists(m_TagDirectory)) {
			AX_CORE_TRACE("TagLoader: tags directory does not exist");
			return;
		}

		LoadDirectory(m_TagDirectory, "");
		ResolveDependencies();

		AX_CORE_INFO("TagLoader: loaded {} tags", m_Tags.size());
	}

	void TagLoader::Reload() {
		AX_CORE_INFO("TagLoader: reloading tags");
		m_Tags.clear();

		if (std::filesystem::exists(m_TagDirectory)) {
			LoadDirectory(m_TagDirectory, "");
			ResolveDependencies();
		}

		AX_CORE_INFO("TagLoader: reloaded {} tags", m_Tags.size());
	}

	const std::vector<std::string>& TagLoader::GetTag(const std::string& tagName) const {
		auto iterator = m_Tags.find(tagName);
		if (iterator != m_Tags.end()) {
			return iterator->second;
		}
		return s_EmptyTag;
	}

	bool TagLoader::IsInTag(const std::string& tagName, const std::string& value) const {
		const auto& entries = GetTag(tagName);
		for (const auto& entry : entries) {
			if (entry == value) {
				return true;
			}
		}
		return false;
	}

	void TagLoader::LoadDirectory(const std::filesystem::path& directory, const std::string& prefix) {
		if (!std::filesystem::exists(directory)) {
			return;
		}

		for (const auto& entry : std::filesystem::directory_iterator(directory)) {
			if (entry.is_directory()) {
				std::string newPrefix = prefix.empty()
					? entry.path().filename().string()
					: prefix + "/" + entry.path().filename().string();
				LoadDirectory(entry.path(), newPrefix);
				continue;
			}

			if (entry.path().extension() != ".json") {
				continue;
			}

			try {
				std::ifstream file(entry.path());
				auto json = nlohmann::json::parse(file);

				std::string tagName = prefix.empty()
					? entry.path().stem().string()
					: prefix + "/" + entry.path().stem().string();

				std::vector<std::string> values;
				if (json.contains("values") && json["values"].is_array()) {
					for (const auto& value : json["values"]) {
						if (value.is_string()) {
							values.push_back(value.get<std::string>());
						}
					}
				}

				bool replace = json.value("replace", false);
				if (replace || m_Tags.find(tagName) == m_Tags.end()) {
					m_Tags[tagName] = std::move(values);
				} else {
					auto& existing = m_Tags[tagName];
					existing.insert(existing.end(), values.begin(), values.end());
				}

				AX_CORE_TRACE("TagLoader: loaded tag '{}'", tagName);
			} catch (const nlohmann::json::exception& exception) {
				AX_CORE_ERROR("TagLoader: failed to parse {}: {}",
					entry.path().filename().string(), exception.what());
			}
		}
	}

	void TagLoader::ResolveDependencies() {
		// Resolve tag references (entries starting with #)
		// by inlining the referenced tag's entries
		bool changed = true;
		int iterations = 0;
		constexpr int maxIterations = 100;

		while (changed && iterations < maxIterations) {
			changed = false;
			++iterations;

			for (auto& [tagName, entries] : m_Tags) {
				std::vector<std::string> resolved;
				for (const auto& entry : entries) {
					if (!entry.empty() && entry[0] == '#') {
						std::string referencedTag = entry.substr(1);
						auto iterator = m_Tags.find(referencedTag);
						if (iterator != m_Tags.end()) {
							resolved.insert(resolved.end(),
								iterator->second.begin(), iterator->second.end());
							changed = true;
						} else {
							AX_CORE_WARN("TagLoader: unresolved tag reference '{}' in '{}'",
								entry, tagName);
						}
					} else {
						resolved.push_back(entry);
					}
				}
				entries = std::move(resolved);
			}
		}

		if (iterations >= maxIterations) {
			AX_CORE_WARN("TagLoader: dependency resolution hit iteration limit — possible cycle");
		}
	}

}
