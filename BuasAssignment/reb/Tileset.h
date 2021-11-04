#ifndef REBOOTTILESET_H
#define REBOOTTILESET_H

#pragma once
#include "OwnedObject.h"
#include <map>
#include <functional>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include "Shader.h"

namespace reb {

	extern enum LayerIndex : int;
	extern enum TileIndex : int;

	class Scene;

	//manages tiles on a single layer
	class Tileset : public OwnedObject<Scene> {
	private:
		//map to convert tile index to tile pointer
		std::map<TileIndex, Tile*> m_tileID;
		//map with the actual tiles
		std::map<std::pair<int, int>, Tile*> m_tiles;

	public:
		//the size of every tile in this set
		const int TILEWIDTH;
		const int TILEHEIGHT;
		//the layer of every tile in this set
		const LayerIndex LAYER;

		//true if this tileset should be checked for collision, false otherwise
		bool m_hasCollision = true;

		//true if the tiles in this tileset should be updates, false otherwise
		bool m_isUpdated = true;

		//constructor
		Tileset(int tileWidth, int tileHeight, LayerIndex layer);

		//uses () operator for easy access to tiles (the returned tile is a clone)
		Tile* operator()(int x, int y)const;

		//registers a tile type
		void registerTile(TileIndex index, Tile* prototype);

		//tile setters
		void setTile(int x, int y, TileIndex tileType);
		void setTiles(std::pair<int, int> boundsX, std::pair<int, int> boundsY, TileIndex tileType);
		void setTiles(std::pair<int, int> origin, std::vector<std::vector<TileIndex>> tiles);
		void setTiles(std::vector<std::pair<int, int>> tiles, TileIndex tileType);
		void setTilesConditional(std::pair<int, int> boundsX, std::pair<int, int> boundsY, TileIndex tileType, std::function<bool(int, int)> shouldSet);
		void setTilesLambda(std::pair<int, int> boundsX, std::pair<int, int> boundsY, std::function<bool(int, int, TileIndex&)> getType);

		//tile removers
		void removeTile(int x, int y);
		void removeTiles(TileIndex tileType);
		void removeTiles(std::pair<int, int> boundsX, std::pair<int, int> boundsY);
		void removeTiles(std::pair<int, int> boundsX, std::pair<int, int> boundsY, TileIndex tileType);
		void removeTilesConditional(std::pair<int, int> boundsX, std::pair<int, int> boundsY, std::function<bool(int, int, TileIndex)> shouldRemove);

		//returns true if the tileset has no tile in the given coords
		bool empty(int x, int y)const;
		//returns the bounds of the tileset in tileset coords
		Rect<int> getTilesetBounds()const;
		//returns the bounds of the tileset in worldspace coords
		Rect<float> getWorldBounds()const;

		//calls the given function for every non-empty tile
		void foreachTile(std::function<void(Tile*const&)> function)const;
		//calls the given function for every non-empty tile within the given bounds
		void foreachTile(std::function<void(Tile*const&)> function, std::pair<int, int> boundsX, std::pair<int, int> boundsY)const;

		//returns true if the given tileindex has been registered in this tileset
		bool registered(TileIndex index);

		//removes the tileset from the scene
		void destroy()override;

		//destructor
		~Tileset();
	};

	//< operator for set sorting
	bool operator<(const Tileset& lhs, const Tileset& rhs);

}

#endif