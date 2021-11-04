#include "HeatTile.h"

//constructor
HeatTile::HeatTile(Level* level, HeatEmitter* heat)
	: reb::Tile(),
	HeatHolder(nullptr, heat),
	m_copyLevel{ level }
{};

//copy constructor
HeatTile::HeatTile(const HeatTile& other)
	: reb::Tile(other),
	HeatHolder(nullptr, other.m_heat->clone()),
	m_copyLevel{ other.m_copyLevel }
{
};

//sets the level if not a prototype when the tile is added to a tileset
void HeatTile::onOwnerSet() {
	if (!isPrototype()) {
		setLevel(m_copyLevel);
	}
}

//returns a clone of the tile
HeatTile* HeatTile::getClone(reb::Tileset& set) {
	auto newTile = new HeatTile(*this);
	newTile->setOwner(&set);
	newTile->copyMainComponentsFrom(*this);
	return newTile;
}