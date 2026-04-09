#include "Axiom/Environment/Level/Dimension.h"

namespace Axiom {

Dimension::Dimension(std::string name, const DimensionType& type, Ref<Level> level)
	: m_Name(std::move(name))
	, m_Type(type)
	, m_Level(std::move(level))
{
}

void Dimension::OnTick(Timestep timestep) {
	if (m_Level) {
		m_Level->OnTick(timestep);
	}
}

bool Dimension::IsOverworld() const {
	return m_Type.Id() == DimensionType::Overworld;
}

bool Dimension::IsNether() const {
	return m_Type.Id() == DimensionType::Nether;
}

bool Dimension::IsEnd() const {
	return m_Type.Id() == DimensionType::End;
}

double Dimension::ScaleFromOverworld(double overworldCoord) const {
	return overworldCoord / m_Type.CoordinateScale();
}

double Dimension::ScaleToOverworld(double dimensionCoord) const {
	return dimensionCoord * m_Type.CoordinateScale();
}

}
