#include "DataVersion.h"

#include <format>

namespace Axiom {

DataVersion::DataVersion(int32_t version)
	: m_Version(version) {}

bool DataVersion::IsCompatible() const {
	return m_Version <= CURRENT_VERSION;
}

bool DataVersion::NeedsMigration() const {
	return m_Version < CURRENT_VERSION;
}

std::string DataVersion::VersionName() const {
	if (m_Version == 3953) return "1.21.4";
	if (m_Version == 3837) return "1.21.2";
	if (m_Version == 3700) return "1.21";
	if (m_Version == 3463) return "1.20.4";
	if (m_Version == 3337) return "1.20";
	return std::format("Unknown (DV {})", m_Version);
}

}
