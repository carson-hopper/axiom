#include "Axiom/Environment/Level/LevelRegistry.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Environment/Level/DimensionType.h"
#include "Axiom/Environment/Level/Generator/FlatChunkGenerator.h"

namespace Axiom {

LevelRegistry::LevelRegistry()
{
}

void LevelRegistry::InitializeVanillaDimensions() {
	// Create Overworld (placeholder until ClassicLevelSource is wired)
	auto overworldGenerator = CreateRef<FlatChunkGenerator>();
	RegisterDimension(
		DimensionType::Overworld,
		DimensionType::GetOverworld(),
		overworldGenerator
	);

	// Create Nether with flat generator (placeholder for now)
	auto netherGenerator = CreateRef<FlatChunkGenerator>();
	RegisterDimension(
		DimensionType::Nether,
		DimensionType::GetNether(),
		netherGenerator
	);

	// Create End with flat generator (placeholder for now)
	auto endGenerator = CreateRef<FlatChunkGenerator>();
	RegisterDimension(
		DimensionType::End,
		DimensionType::GetEnd(),
		endGenerator
	);
}

Ref<Dimension> LevelRegistry::RegisterDimension(
	const std::string& name,
	const DimensionType& type,
	Ref<ChunkGenerator> generator
) {
	std::lock_guard<std::mutex> lock(m_Mutex);

	if (m_Dimensions.find(name) != m_Dimensions.end()) {
		AX_CORE_WARN("LevelRegistry: Dimension '{}' already exists", name);
		return nullptr;
	}

	auto level = CreateRef<Level>(name, generator);
	auto dimension = CreateRef<Dimension>(name, type, level);
	m_Dimensions[name] = dimension;

	// Cache vanilla dimensions for quick access
	if (name == DimensionType::Overworld) {
		m_Overworld = dimension;
	} else if (name == DimensionType::Nether) {
		m_Nether = dimension;
	} else if (name == DimensionType::End) {
		m_End = dimension;
	}

	AX_CORE_INFO("LevelRegistry: Registered dimension '{}' (type: {})", name, type.Id());
	return dimension;
}

bool LevelRegistry::UnregisterDimension(const std::string& name) {
	std::lock_guard<std::mutex> lock(m_Mutex);

	auto it = m_Dimensions.find(name);
	if (it == m_Dimensions.end()) {
		return false;
	}

	// Clear cache if it's a vanilla dimension
	if (name == DimensionType::Overworld) {
		m_Overworld.Reset();
	} else if (name == DimensionType::Nether) {
		m_Nether.Reset();
	} else if (name == DimensionType::End) {
		m_End.Reset();
	}

	m_Dimensions.erase(it);
	AX_CORE_INFO("LevelRegistry: Unregistered dimension '{}'", name);
	return true;
}

Ref<Dimension> LevelRegistry::GetDimension(const std::string& name) {
	std::lock_guard<std::mutex> lock(m_Mutex);

	auto it = m_Dimensions.find(name);
	if (it != m_Dimensions.end()) {
		return it->second;
	}
	return nullptr;
}

Ref<Dimension> LevelRegistry::GetOverworld() {
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_Overworld;
}

Ref<Dimension> LevelRegistry::GetNether() {
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_Nether;
}

Ref<Dimension> LevelRegistry::GetEnd() {
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_End;
}

std::vector<Ref<Dimension>> LevelRegistry::GetAllDimensions() {
	std::lock_guard<std::mutex> lock(m_Mutex);

	std::vector<Ref<Dimension>> result;
	result.reserve(m_Dimensions.size());
	for (const auto& [name, dimension] : m_Dimensions) {
		result.push_back(dimension);
	}
	return result;
}

bool LevelRegistry::HasDimension(const std::string& name) {
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_Dimensions.find(name) != m_Dimensions.end();
}

size_t LevelRegistry::GetDimensionCount() const {
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_Dimensions.size();
}

void LevelRegistry::OnTick(Timestep timestep) {
	std::lock_guard<std::mutex> lock(m_Mutex);

	for (auto& [name, dimension] : m_Dimensions) {
		if (dimension) {
			dimension->OnTick(timestep);
		}
	}
}

}
