#pragma once

#include "Axiom/Core/Base.h"
#include "Axiom/Core/Tickable.h"
#include "Axiom/Environment/Level/DimensionType.h"
#include "Axiom/Environment/Level/Level.h"

namespace Axiom {

	class Level;

	/**
	 * A Dimension represents a specific instance of a dimension type.
	 * 
	 * Each dimension has a type (Overworld, Nether, End) that defines its
	 * properties, and a Level that contains the actual world data (chunks,
	 * entities, etc.).
	 * 
	 * Multiple dimensions can share the same type but have different Levels.
	 * This allows plugins to create custom dimensions with vanilla properties.
	 */
	class Dimension : public Tickable {
	public:
		Dimension(std::string name, const DimensionType& type, Ref<Level> level);

		const std::string& Name() const { return m_Name; }
		const DimensionType& Type() const { return m_Type; }
		Ref<Level> GetLevel() const { return m_Level; }

		/**
		 * Called every game tick. Delegates to the Level.
		 */
		void OnTick(Timestep timestep) override;

		/**
		 * Check if this is the Overworld.
		 */
		bool IsOverworld() const;

		/**
		 * Check if this is the Nether.
		 */
		bool IsNether() const;

		/**
		 * Check if this is the End.
		 */
		bool IsEnd() const;

		/**
		 * Converts overworld coordinates to this dimension's coordinates.
		 */
		double ScaleFromOverworld(double overworldCoord) const;

		/**
		 * Converts this dimension's coordinates to overworld coordinates.
		 */
		double ScaleToOverworld(double dimensionCoord) const;

	private:
		std::string m_Name;
		const DimensionType& m_Type;
		Ref<Level> m_Level;
	};

}
