#ifndef REBOOTTILEMAP_H
#define REBOOTTILEMAP_H

#pragma once
#include "Content.h"
#include "Tileset.h"
#include "Scene.h"
#include "Material.h"

namespace reb {

	extern enum TileIndex : int;
	class ContentLoader;

	//used to load tilemaps generated with tiled
	class Tilemap {
		friend ContentLoader;
	private:
		//struct for better access to tilemap layers
		struct TilemapLayer {
		public:
			//basic info
			const int ID;
			const std::string NAME;
			const int WIDTH;
			const int HEIGHT;
			const bool SOLID;
			const bool SHADED;

			//the actual map
			const std::vector<std::vector<int>> GIDMAP;

			//constructor
			TilemapLayer(int id, std::string name, int width, int height, bool solid, bool shaded, std::vector<std::vector<int>> gidMap);

			//subscript operator for easy access
			const std::vector<int>& operator[](int k);
		};

		//constants for content file locations
		const std::string CONTENTFOLDER = "Content/";
		const std::string CONTENTEXTENSION = ".rebtm";

		//constant for an empty tile
		const int EMPTYTILE = 0;

		//map of content used by the tilemap indexed by their gid
		std::map<int, Content*> m_contents;
		std::vector<TilemapLayer> m_layers;

		//a map of tileindexes indexed by gid's
		std::map<int, TileIndex> m_indexByGid;

		//a map of tile prototypes that should be used instead of any automatically generated ones indexed by their tileIndex
		std::map<TileIndex, Tile*> m_specificPrototypes;

		//tilesize
		int m_tileWidth;
		int m_tileHeight;

	public:
		//constructor
		Tilemap(std::string fileLocation, std::vector<std::pair<TileIndex, TileIndex>> tileRanges);

		//adds all the tiles in the tilemap to the referenced tileset
		Tileset& addMap(Scene* scene, std::string tilemapLayer, LayerIndex renderLayer, Material tileMaterial = Material{0, 0, 0, 0});

		//specifies a tile prototype for the the given tile index
		void specifyPrototype(TileIndex index, Tile* prototype);

		//content getters
		Content* getContent(TileIndex tileType);
		Content* getContent(int gid);

		//destructor
		~Tilemap();
	};

}

#endif