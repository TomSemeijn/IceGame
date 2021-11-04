#include "Tilemap.h"
#include <rapidxml/rapidxml_utils.hpp>
#include "Functions.h"
#include "ShadedSprite.h"
#include "Rigidbody.h"
#include "Concave.h"

namespace reb {

	//constructor for TilemapLayer object 
	Tilemap::TilemapLayer::TilemapLayer(int id, std::string name, int width, int height, bool solid, bool shaded, std::vector<std::vector<int>> gidMap)
		: 
		ID{ id }, 
		NAME{ name }, 
		WIDTH{ width }, 
		HEIGHT{ height },
		GIDMAP{ gidMap },
		SOLID{ solid },
		SHADED{ shaded }
	{};

	//subscript operator for easy access to TilemapLayer object
	const std::vector<int>& Tilemap::TilemapLayer::operator[](int k) {
		return GIDMAP[k];
	}

	//constructor
	Tilemap::Tilemap(std::string fileLocation, std::vector<std::pair<TileIndex, TileIndex>> tileRanges) {

		//adds constant modifiers to the file location
		fileLocation = CONTENTFOLDER + fileLocation + CONTENTEXTENSION;

		//tries to open the given file as a rapidxml doc
		rapidxml::file<> file(fileLocation.data());
		//parses the xml file
		rapidxml::xml_document<> doc;
		doc.parse<0>(file.data());

		//gets root node
		rapidxml::xml_node<>* root = doc.first_node("root");

		//loads and indexes all the content files
		{
			//gets the path of the indexer file
			std::filesystem::path indexPath(fileLocation);

			//gets the folder the indexer file is in
			std::filesystem::path indexFolder = indexPath.parent_path();

			//gets the indexer node
			rapidxml::xml_node<>* indexer = root->first_node("indexer");

			//goes through all file nodes
			rapidxml::xml_node<>* file = indexer->first_node("file");
			do {
				//gets attributes as variables
				int gid = std::stoi(file->first_attribute("gid")->value());
				std::string rebLocation = file->first_attribute("file")->value();

				//gets the global location of the rebfile
				rebLocation = getGlobalPath(rebLocation, indexFolder);

				//loads the content
				m_contents.emplace(std::make_pair(gid, new Content(rebLocation)));

			} while (file = file->next_sibling("file"));

		}
		//sets up the map
		{
			//gets the map node
			rapidxml::xml_node<>* map = root->first_node("map");

			//sets tile size members
			m_tileWidth = std::stoi(map->first_attribute("tileWidth")->value());
			m_tileHeight = std::stoi(map->first_attribute("tileHeight")->value());

			//goes through all the layers
			rapidxml::xml_node<>* layerNode = map->first_node("layer");
			do {
				//gets the info as variables
				int layerID = std::stoi(layerNode->first_attribute("id")->value());
				std::string layerName = layerNode->first_attribute("name")->value();
				int layerWidth = std::stoi(layerNode->first_attribute("width")->value());
				int layerHeight = std::stoi(layerNode->first_attribute("height")->value());
				bool layerSolid = std::string(layerNode->first_attribute("solid")->value()) == "true";
				bool layerShaded = std::string(layerNode->first_attribute("shaded")->value()) == "true";

				//sets up a vector for the gid's filled with emptyTile values
				std::vector<std::vector<int>> gidMap{};
				gidMap.reserve(layerWidth);
				for (int k = 0; k < layerWidth; k++) {
					gidMap.emplace_back(layerHeight);
					std::fill(gidMap.back().begin(), gidMap.back().end(), EMPTYTILE);
				}

				//goes through all the tiles and puts them into the map
				rapidxml::xml_node<>* dataNode = layerNode->first_node("data");
				
				std::vector<std::string> gidData = split(dataNode->value(), ',');

				//adds the found gidmap to the vector
				//m_layers.emplace_back(layerID, layerName, layerWidth, layerHeight, layerSolid, layerShaded, gidMap);

				int gidIndex = 0;
				for (int y = 0; y < layerHeight; y++) {
					for (int x = 0; x < layerWidth; x++) {
						gidMap[x][y] = std::stoi(gidData[gidIndex]);
						gidIndex++;
					}
				}
				gidMap.shrink_to_fit();
				m_layers.emplace_back(layerID, layerName, layerWidth, layerHeight, layerSolid, layerShaded, gidMap);

			} while (layerNode = layerNode->next_sibling("layer"));

		}

		//sets up gid map
		{
			//loops through all the tile indexes within the given ranges
			auto currentGidIter = m_contents.begin();
			for (auto& range : tileRanges) {
				for (int k = (int)range.first; k <= (int)range.second; k++) {
					m_indexByGid.emplace(std::make_pair(currentGidIter->first, (TileIndex)k));
					currentGidIter++;
				}
			}

		}

	}

	//adds all the tiles in the tilemap to the referenced tileset
	Tileset& Tilemap::addMap(Scene* scene, std::string tilemapLayer, LayerIndex renderLayer, Material tileMaterial) {

		//verifies the scene doesnt already have a tileset on the given layer
		if (scene->hasTileset(renderLayer)) {
			throw std::out_of_range("The renderlayer passed to reb::Tilemap::addMap already has a tileset in the passed reb::Scene");
		}

		//adds the tileset
		Tileset& set = scene->addTileset(m_tileWidth, m_tileHeight, renderLayer);

		//finds the tilemap layer
		TilemapLayer& mapLayer = *(std::find_if(m_layers.begin(), m_layers.end(), [&tilemapLayer](TilemapLayer& element) {
			return element.NAME == tilemapLayer;
		}));

		//adds all the tiles in the layer, registering them as we go
		for (int x = 0; x < mapLayer.WIDTH; x++) {
			for (int y = 0; y < mapLayer.HEIGHT; y++) {

				//gets gid of the current tile
				int gid = mapLayer[x][y];

				//checks if the tile is empty or not
				if (gid != EMPTYTILE) {

					//gets tileindex of the current tile
					TileIndex tileIndex = m_indexByGid.at(gid);

					//registers the tile if necessary
					if (!set.registered(tileIndex)) {

						//initializes prototype variable
						Tile* prototype = nullptr;

						//tries to find specified prototype
						auto specified = m_specificPrototypes.find(tileIndex);

						//uses specifies prototype if one was given
						if (specified != m_specificPrototypes.end()) {
							prototype = specified->second->getClone(set);
							prototype->m_isPrototype = true;
						}
						//automatically generates a prototype otherwise
						else {
							//creates prototype sprite
							Drawable* spr = nullptr;
							if (mapLayer.SHADED) {
								spr = new ShadedSprite(m_contents.at(gid));
							}
							else {
								spr = new Sprite(m_contents.at(gid));
							}
							//creates prototype rigidbody
							Rigidbody* bod = nullptr;
							if (mapLayer.SOLID) {
								bod = new Rigidbody(new Concave(m_contents.at(gid)), tileMaterial, 0, true);
							}
							//creates the prototype
							prototype = new Tile();
							prototype->setSprite(spr);
							prototype->setBody(bod);
						}

						//registers with prototype
						set.registerTile(tileIndex, prototype);

					}
					//sets the tile
					set.setTile(x, y, tileIndex);
				}

			}
		}

		//returns the set
		return set;
		
	}

	//specifies a tile prototype for the the given tile index
	void Tilemap::specifyPrototype(TileIndex index, Tile* prototype) {
		//checks if the index has already been specified
		auto proto = m_specificPrototypes.find(index);
		if (proto != m_specificPrototypes.end()) {
			//frees memory of the previous prototype
			delete proto->second;
			//erases from the map
			m_specificPrototypes.erase(proto);
		}
		//adds the prototype to the map
		m_specificPrototypes.emplace(std::make_pair(index, prototype));
	}

	//returns the content saved at the given tile type
	Content* Tilemap::getContent(TileIndex tileType) {
		//finds the gid with the given index
		int gid = 0;
		for (auto& pair : m_indexByGid) {
			if (pair.second == tileType) { gid = pair.first; break; }
		}
		return getContent(gid);
	}

	//returns the content saved at the given gid
	Content* Tilemap::getContent(int gid) {
		return m_contents.at(gid);
	}

	//destructor
	Tilemap::~Tilemap() {
		//frees memory of the specified tile prototype
		for (auto& proto : m_specificPrototypes) {
			delete proto.second;
		}
		//frees memory of all the content
		for (auto& content : m_contents) {
			delete content.second;
		}
	}

}