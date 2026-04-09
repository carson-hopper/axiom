#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>

namespace Axiom {

	/**
	 * Minecraft namespace:path identifier
	 * (e.g. "minecraft:stone",
	 * "axiom:custom_block").
	 */
	class ResourceLocation {
	public:
		static constexpr std::string_view DEFAULT_NAMESPACE = "minecraft";

		ResourceLocation() = default;

		ResourceLocation(std::string namespaceName, std::string path)
			: m_Namespace(std::move(namespaceName))
			, m_Path(std::move(path)) {}

		/**
		 * Parses a combined "namespace:path"
		 * string. Uses the default
		 * namespace if no colon
		 * is present.
		 */
		explicit ResourceLocation(const std::string& combined) {
			auto separator = combined.find(':');
			if (separator == std::string::npos) {
				m_Namespace = std::string(DEFAULT_NAMESPACE);
				m_Path = combined;
			} else {
				m_Namespace = combined.substr(0, separator);
				m_Path = combined.substr(separator + 1);
			}
		}

		const std::string& Namespace() const { return m_Namespace; }
		const std::string& Path() const { return m_Path; }

		/**
		 * Returns the full
		 * "namespace:path"
		 * string form.
		 */
		std::string ToString() const {
			return m_Namespace + ":" + m_Path;
		}

		bool operator==(const ResourceLocation&) const = default;

		bool operator<(const ResourceLocation& other) const {
			if (m_Namespace != other.m_Namespace) {
				return m_Namespace < other.m_Namespace;
			}
			return m_Path < other.m_Path;
		}

		/**
		 * Validates that a namespace string contains only
		 * lowercase letters, digits, underscores,
		 * hyphens, and periods.
		 */
		static bool IsValidNamespace(std::string_view text) {
			if (text.empty()) {
				return false;
			}
			for (char character : text) {
				if (!((character >= 'a' && character <= 'z') ||
					  (character >= '0' && character <= '9') ||
					  character == '_' || character == '-' || character == '.')) {
					return false;
				}
			}
			return true;
		}

		/**
		 * Validates that a path string contains only
		 * lowercase letters, digits, underscores,
		 * hyphens, periods, and
		 * forward slashes.
		 */
		static bool IsValidPath(std::string_view text) {
			if (text.empty()) {
				return false;
			}
			for (char character : text) {
				if (!((character >= 'a' && character <= 'z') ||
					  (character >= '0' && character <= '9') ||
					  character == '_' || character == '-' ||
					  character == '.' || character == '/')) {
					return false;
				}
			}
			return true;
		}

		/**
		 * Returns true if both namespace
		 * and path are valid.
		 */
		bool IsValid() const {
			return IsValidNamespace(m_Namespace) && IsValidPath(m_Path);
		}

	private:
		std::string m_Namespace = "minecraft";
		std::string m_Path;
	};

}

template<>
struct std::hash<Axiom::ResourceLocation> {
	std::size_t operator()(const Axiom::ResourceLocation& location) const noexcept {
		std::size_t namespaceHash = std::hash<std::string>{}(location.Namespace());
		std::size_t pathHash = std::hash<std::string>{}(location.Path());
		return namespaceHash ^ (pathHash << 1);
	}
};
