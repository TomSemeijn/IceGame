#ifndef REBOOTTILE_H
#define REBOOTTILE_H

#pragma once
#include "Container.h"
#include <memory>

namespace reb {

	class Tileset;
	class Tilemap;

	enum TileIndex : int;

	//a tile, owned by a tileset, which is owned by the scene
	//could also be a prototype in which case it's used to clone other tiles from
	class Tile : public Container, public OwnedObject<Tileset> {
		//sets the tileset as friend so it can force the position of the tile, allowing one tile to define many
		friend Tileset;
		friend Tilemap;
		using OwnedObject<Tileset>::destroy;
	private:
		int m_posX = 0;
		int m_posY = 0;
		TileIndex m_index = (TileIndex)0;
		bool m_isPrototype;
	protected:
		//returns true if the tile is a prototype
		bool isPrototype();

		//copy constructor
		Tile(const Tile &obj);
	public:
		//constructor
		Tile();

		//returns a clone of the tile
		virtual Tile* getClone(Tileset& addTo);

		//returns the position of the tile within the scene
		std::pair<int, int> getTilePos();

		//returns the index of the tile
		TileIndex getTileIndex();

		//removes the container from the scene
		virtual void destroy()override;

		//returns the scene
		virtual Scene* getScene()override;

	};

}

#endif