#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>
#include <string>

namespace Axiom {

	/**
	 * Defines the immutable properties of a dimension type.
	 * 
	 * Minecraft has three built-in dimension types: Overworld, Nether, and End.
	 * Plugins can register custom dimension types with unique properties.
	 */
	class DimensionType {
	public:
		/**
		 * Built-in dimension type identifiers.
		 */
		static constexpr const char* Overworld = "minecraft:overworld";
		static constexpr const char* Nether = "minecraft:the_nether";
		static constexpr const char* End = "minecraft:the_end";

		/**
		 * Creates a dimension type with specified properties.
		 */
		DimensionType(
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
		);

		const std::string& Id() const { return m_Id; }
		int32_t MinY() const { return m_MinY; }
		int32_t Height() const { return m_Height; }
		int32_t MaxY() const { return m_MinY + m_Height; }
		bool HasSkylight() const { return m_HasSkylight; }
		bool HasCeiling() const { return m_HasCeiling; }
		bool UltraWarm() const { return m_UltraWarm; }
		bool Natural() const { return m_Natural; }
		double CoordinateScale() const { return m_CoordinateScale; }
		bool PiglinSafe() const { return m_PiglinSafe; }
		bool BedWorks() const { return m_BedWorks; }
		bool RespawnAnchorWorks() const { return m_RespawnAnchorWorks; }
		bool HasRaids() const { return m_HasRaids; }
		int32_t LogicalHeight() const { return m_LogicalHeight; }
		const std::string& Infiniburn() const { return m_Infiniburn; }
		float AmbientLight() const { return m_AmbientLight; }

		/**
		 * Gets the built-in Overworld dimension type.
		 */
		static const DimensionType& GetOverworld();

		/**
		 * Gets the built-in Nether dimension type.
		 */
		static const DimensionType& GetNether();

		/**
		 * Gets the built-in End dimension type.
		 */
		static const DimensionType& GetEnd();

	private:
		std::string m_Id;
		int32_t m_MinY;
		int32_t m_Height;
		bool m_HasSkylight;
		bool m_HasCeiling;
		bool m_UltraWarm;
		bool m_Natural;
		double m_CoordinateScale;
		bool m_PiglinSafe;
		bool m_BedWorks;
		bool m_RespawnAnchorWorks;
		bool m_HasRaids;
		int32_t m_LogicalHeight;
		std::string m_Infiniburn;
		float m_AmbientLight;
	};

}
