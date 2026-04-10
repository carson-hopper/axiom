#pragma once

#include <string>

namespace Axiom {

	/**
	 * Check whether an owned permission pattern matches a required
	 * permission string. Supports Bukkit-style wildcards:
	 *
	 *   - `"*"`                         → matches anything
	 *   - `"axiom.*"`                   → matches any permission whose
	 *                                     name starts with `axiom.`
	 *                                     (strict child, e.g. `axiom.foo`)
	 *   - `"axiom.gamemode.*"`          → matches `axiom.gamemode.creative`,
	 *                                     `axiom.gamemode.survival`, etc.
	 *   - `"axiom.gamemode.creative"`   → exact match
	 *
	 * Wildcards must appear at the end of the pattern. A pattern like
	 * `"axiom.*.creative"` is NOT supported; it only matches literally.
	 *
	 * @param owned    The permission the subject has been granted,
	 *                 possibly containing a trailing wildcard.
	 * @param required The exact permission the command is checking for.
	 * @return true when @p owned grants @p required.
	 */
	inline bool MatchesPermissionPattern(const std::string& owned,
		const std::string& required) {

		if (owned == required) {
			return true;
		}
		if (owned == "*") {
			return true;
		}
		if (owned.ends_with(".*")) {
			// Strip the trailing '*' to get the prefix including the dot,
			// e.g. "axiom.gamemode.*" → "axiom.gamemode."
			const std::string prefix = owned.substr(0, owned.size() - 1);
			return required.size() > prefix.size() && required.starts_with(prefix);
		}
		return false;
	}

}
