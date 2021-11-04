#include "Tileset.h"
#include "Sprite.h"
#include "Scene.h"

namespace reb {

	//constructor
	Tileset::Tileset(int tileWidth, int tileHeight, LayerIndex layer) : TILEWIDTH{ tileWidth }, TILEHEIGHT{ tileHeight }, LAYER{ layer } {}

	//< operator for set sorting
	bool operator<(const Tileset& lhs, const Tileset& rhs) {
		return lhs.LAYER < rhs.LAYER;
	}

	//uses () operator for easy access to tiles (the returned tile is a clone)
	Tile* Tileset::operator()(int x, int y)const {
		//tries to find a tileID with the given coords
		auto tileIndexIter = m_tiles.find(std::make_pair(x, y));
		//if the tile wasnt found
		if (tileIndexIter == m_tiles.end()) { return nullptr; }
		//gets tile if it was found
		return tileIndexIter->second;
	}

	//registers a tile type
	void Tileset::registerTile(TileIndex index, Tile* prototype) {
		prototype->setLayer(LAYER);
		m_tileID.emplace(std::make_pair(index, prototype));
	}

	//sets a single tile
	void Tileset::setTile(int x, int y, TileIndex tileType) {
		//removes the tile in case there is already one
		removeTile(x, y);

		//creates a clone of the prototype with this index
		Tile* tile = m_tileID.at(tileType);
		//gets the clone of the tile
		auto clone = tile->getClone(*this);
		//sets the position of the tile to the requested coords
		clone->forcePos(Vector2(x * TILEWIDTH, y * TILEHEIGHT));
		//sets the posX and posY members
		clone->m_posX = x;
		clone->m_posY = y;
		//sets the index of the tile
		clone->m_index = tileType;

		//sets the new tile
		m_tiles.emplace(std::make_pair(x, y), clone);

	}

	//sets all tiles in the given bounds to the new tile
	void Tileset::setTiles(std::pair<int, int> boundsX, std::pair<int, int> boundsY, TileIndex tileType) {
		//loops through every tile within the bounds
		for (int x = boundsX.first; x <= boundsX.second; x++) {
			for (int y = boundsY.first; y <= boundsY.second; y++) {
				//sets the tile
				setTile(x, y, tileType);
			}
		}
	}

	//sets all the tiles in the given vector from the given origin
	void Tileset::setTiles(std::pair<int, int> origin, std::vector<std::vector<TileIndex>> tiles) {
		for (int y = 0; y < tiles.size(); y++) {
			int currentY = y + origin.second;
			for (int x = 0; x < tiles[y].size(); x++) {
				setTile(x + origin.first, currentY, tiles[y][x]);
			}
		}
	}

	//sets all the tiles at the positions given by the vector to the given type
	void Tileset::setTiles(std::vector<std::pair<int, int>> tiles, TileIndex tileType) {
		for (auto& pos : tiles) {
			setTile(pos.first, pos.second, tileType);
		}
	}

	//sets every tile within the bounds to the given index if the given function evaluates to true for the current coordinates
	void Tileset::setTilesConditional(std::pair<int, int> boundsX, std::pair<int, int> boundsY, TileIndex tileType, std::function<bool(int, int)> shouldSet) {
		//loops through all coords of the bounds
		for (int x = boundsX.first; x <= boundsX.second; x++) {
			for (int y = boundsY.first; y <= boundsY.second; y++) {
				//checks if the tile should be set
				if (shouldSet(x, y)) {
					//sets the tile
					setTile(x, y, tileType);
				}
			}
		}
	}

	//sets every tile within the bounds to the index the given function evaluates to
	void Tileset::setTilesLambda(std::pair<int, int> boundsX, std::pair<int, int> boundsY, std::function<bool(int, int, TileIndex&)> getType) {
		//loops through all coords of the bounds
		for (int x = boundsX.first; x <= boundsX.second; x++) {
			for (int y = boundsY.first; y <= boundsY.second; y++) {
				//sets the tile
				TileIndex newTile;
				if (getType(x, y, newTile)) {
					setTile(x, y, newTile);
				}
			}
		}
	}

	//removes one specific tile
	void Tileset::removeTile(int x, int y) {
		m_tiles.erase(std::make_pair(x, y));
	}

	//removes every tile with the given type
	void Tileset::removeTiles(TileIndex tileType) {

		//sets up lambda for finding a tile with the given tile index
		auto tileFind = [&tileType](const std::pair<std::pair<int, int>, Tile*>& element) {
			return element.second->getTileIndex() == tileType;
		};

		//finds every relevant tile
		auto tileIter = std::find_if(m_tiles.begin(), m_tiles.end(), tileFind);
		while (tileIter != m_tiles.end()) {

			//removes the tile
			removeTile(tileIter->first.first, tileIter->first.second);

			//resets iterator
			tileIter = std::find_if(m_tiles.begin(), m_tiles.end(), tileFind);

		}
	}

	//removes every tile within the given bounds
	void Tileset::removeTiles(std::pair<int, int> boundsX, std::pair<int, int> boundsY) {
		//loops through all coords within the bounds
		for (int x = boundsX.first; x <= boundsX.second; x++) {
			for (int y = boundsY.first; y <= boundsY.second; y++) {
				//removes tile
				removeTile(x, y);
			}
		}
	}

	//removes every tile of the given type within the given bounds
	void Tileset::removeTiles(std::pair<int, int> boundsX, std::pair<int, int> boundsY, TileIndex tileType) {
		//loops through all coords within the bounds
		for (int x = boundsX.first; x <= boundsX.second; x++) {
			for (int y = boundsY.first; y <= boundsY.second; y++) {
				//verifies the tile isnt empty and has the requested type
				if (!empty(x, y) && m_tiles.find(std::make_pair(x, y))->second->getTileIndex() == tileType) {
					//removes the tile
					removeTile(x, y);
				}
			}
		}
	}

	//removes every tile within the given bounds where the given function evaluates to true
	void Tileset::removeTilesConditional(std::pair<int, int> boundsX, std::pair<int, int> boundsY, std::function<bool(int, int, TileIndex)> shouldRemove) {
		//loops through all coords within the bounds
		for (int x = boundsX.first; x <= boundsX.second; x++) {
			for (int y = boundsY.first; y <= boundsY.second; y++) {
				//verifies the tile isnt empty and the given function evaluates to true
				if (!empty(x, y) && shouldRemove(x, y, m_tiles.find(std::make_pair(x, y))->second->getTileIndex())) {
					//removes the tile
					removeTile(x, y);
				}
			}
		}
	}

	//returns true if the tileset has no tile in the given coords
	bool Tileset::empty(int x, int y)const {
		return m_tiles.find(std::make_pair(x, y)) == m_tiles.end();
	}

	//returns the bounds of the tileset in tileset coords [min => max>
	Rect<int> Tileset::getTilesetBounds()const {
		//returns empty rectangle if there are no tiles
		if (m_tiles.empty()) { return Rect<int>(); }
		//gets first tile
		auto tileIter = m_tiles.begin();
		//gets initial bounds
		int left = tileIter->first.first;
		int right = tileIter->first.first;
		int top = tileIter->first.second;
		int bottom = tileIter->first.second;
		//iterates through all tiles to find real bounds
		tileIter++;
		while (tileIter != m_tiles.end()) {
			left = std::min(left, tileIter->first.first);
			right = std::max(right, tileIter->first.first);
			top = std::min(top, tileIter->first.second);
			bottom = std::max(bottom, tileIter->first.second);
			tileIter++;
		}
		//increments right and bottom by one to include the tiles themselves
		right++;
		bottom++;
		//returns the found bounds in rectangle form
		return Rect<int>(left, top, right - left, bottom - top);
	}

	//returns the bounds of the tileset in worldspace coords
	Rect<float> Tileset::getWorldBounds()const {
		//gets tileset coord bounds
		Rect<int> tileBounds = getTilesetBounds();
		//returns worldspace coord version
		return Rect<float>(
			tileBounds.left * TILEWIDTH, 
			tileBounds.top * TILEHEIGHT, 
			tileBounds.width * TILEWIDTH,
			tileBounds.height * TILEHEIGHT
		);
	}

	//calls the given function for every non-empty tile
	void Tileset::foreachTile(std::function<void(Tile*const&)> function)const {
		for (auto& tileInfo : m_tiles) {
			function(tileInfo.second);
		}
	}

	//calls the given function for every non-empty tile within the given bounds
	void Tileset::foreachTile(std::function<void(Tile*const&)> function, std::pair<int, int> boundsX, std::pair<int, int> boundsY)const {
		for (int x = boundsX.first; x < boundsX.second; x++) {
			for (int y = boundsY.first; y < boundsY.second; y++) {
				if (!empty(x, y)) {
					function((*this)(x, y));
				}
			}
		}
	}

	//returns true if the given tileindex has been registered in this tileset
	bool Tileset::registered(TileIndex index) {
		return m_tileID.find(index) != m_tileID.end();
	}

	//removes the tileset from the scene
	void Tileset::destroy() {
		if (hasOwner()) {
			getOwner()->removeTileset(LAYER);
		}
	}

	//destructor
	Tileset::~Tileset() {
		//frees memory of every tile prototype
		for (auto& tile : m_tileID) {
			delete tile.second;
		}
		//frees memory of every tile
		for (auto& tile : m_tiles) {
			delete tile.second;
		}
	}

}