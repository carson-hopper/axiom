#include "Axiom/Environment/Level/DimensionType.h"

namespace Axiom {

DimensionType::DimensionType(
	std::string id,
	int32_t minY,
	int32_t height,
	bool hasSkylight,
	bool hasCeiling,
	bool ultraWarm,
	bool natural,
	double coordinateScale,
	bool piglinSafe,
	bool bedWorks,
	bool respawnAnchorWorks,
	bool hasRaids,
	int32_t logicalHeight,
	std::string infiniburn,
	float ambientLight
)
	: m_Id(std::move(id))
	, m_MinY(minY)
	, m_Height(height)
	, m_HasSkylight(hasSkylight)
	, m_HasCeiling(hasCeiling)
	, m_UltraWarm(ultraWarm)
	, m_Natural(natural)
	, m_CoordinateScale(coordinateScale)
	, m_PiglinSafe(piglinSafe)
	, m_BedWorks(bedWorks)
	, m_RespawnAnchorWorks(respawnAnchorWorks)
	, m_HasRaids(hasRaids)
	, m_LogicalHeight(logicalHeight)
	, m_Infiniburn(std::move(infiniburn))
	, m_AmbientLight(ambientLight)
{
}

const DimensionType& DimensionType::GetOverworld() {
	static DimensionType overworld(
		"minecraft:overworld",
		-64,    // minY
		384,    // height
		true,   // hasSkylight
		false,  // hasCeiling
		false,  // ultraWarm
		true,   // natural
		1.0,    // coordinateScale
		false,  // piglinSafe
		true,   // bedWorks
		false,  // respawnAnchorWorks
		true,   // hasRaids
		384,    // logicalHeight
		"minecraft:infiniburn_overworld",
		0.0f    // ambientLight
	);
	return overworld;
}

const DimensionType& DimensionType::GetNether() {
	static DimensionType nether(
		"minecraft:the_nether",
		0,      // minY
		256,    // height
		false,  // hasSkylight
		true,   // hasCeiling
		true,   // ultraWarm
		false,  // natural
		8.0,    // coordinateScale
		true,   // piglinSafe
		false,  // bedWorks
		true,   // respawnAnchorWorks
		false,  // hasRaids
		128,    // logicalHeight
		"minecraft:infiniburn_nether",
		0.1f    // ambientLight
	);
	return nether;
}

const DimensionType& DimensionType::GetEnd() {
	static DimensionType end(
		"minecraft:the_end",
		0,      // minY
		256,    // height
		false,  // hasSkylight
		false,  // hasCeiling
		false,  // ultraWarm
		false,  // natural
		1.0,    // coordinateScale
		false,  // piglinSafe
		false,  // bedWorks
		false,  // respawnAnchorWorks
		true,   // hasRaids
		256,    // logicalHeight
		"minecraft:infiniburn_end",
		0.0f    // ambientLight
	);
	return end;
}

}
