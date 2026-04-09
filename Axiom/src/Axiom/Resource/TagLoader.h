#pragma once

#include "Axiom/Core/Base.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace Axiom {

	/**
	 * Loads and resolves tags with proper
	 * dependency ordering. Tags may reference
	 * other tags using the # prefix, which
	 * requires topological resolution.
	 */
	class TagLoader {
	public:
		TagLoader() = default;
		~TagLoader() = default;

		/**
		 * Load all tags from the
		 * given directory.
		 */
		void Load(const std::filesystem::path& tagDirectory);

		/**
		 * Reload and re-resolve
		 * all tags.
		 */
		void Reload();

		/**
		 * Get the resolved entries for
		 * a named tag. Returns empty
		 * if the tag does not exist.
		 */
		const std::vector<std::string>& GetTag(const std::string& tagName) const;

		/**
		 * Check whether a specific value
		 * is contained in a tag.
		 */
		bool IsInTag(const std::string& tagName, const std::string& value) const;

		size_t TagCount() const { return m_Tags.size(); }

	private:
		void LoadDirectory(const std::filesystem::path& directory, const std::string& prefix);
		void ResolveDependencies();

		std::filesystem::path m_TagDirectory;
		std::unordered_map<std::string, std::vector<std::string>> m_Tags;
		static const std::vector<std::string> s_EmptyTag;
	};

}
