#include "Tile.h"
#include "Tileset.h"

#include "Drawable.h"
#include "ShadedSprite.h"

namespace reb{

	//constructor
	Tile::Tile() : Container{ } {
		m_isPrototype = true;
	};

	//copy constructor
	Tile::Tile(const Tile& original) 
		:	Container(original) 
	{
		m_isPrototype = false;
	}

	//returns a unique pointer clone of the tile
	Tile* Tile::getClone(Tileset& addTo) {
		Tile* clone = new Tile(*this);
		clone->setOwner(&addTo);
		clone->copyMainComponentsFrom(*this);
		return clone;
	}

	//returns the position of the tile within the scene
	std::pair<int, int> Tile::getTilePos() {
		return std::make_pair(m_posX, m_posY);
	}

	//returns the index of the tile
	TileIndex Tile::getTileIndex() {
		return m_index;
	}

	//returns true if the tile is a prototype
	bool Tile::isPrototype() {
		return m_isPrototype;
	}

	//removes the container from the scene
	void Tile::destroy() {
		if (OwnedObject<Tileset>::hasOwner()) {
			OwnedObject<Tileset>::getOwner()->removeTile(m_posX, m_posY);
		}
	}

	//returns the scene
	Scene* Tile::getScene() {
		if (hasOwner()) {
			auto owner = getOwner();
			if (owner->hasOwner()) {
				return owner->getOwner();
			}
		}
		return nullptr;
	}

}