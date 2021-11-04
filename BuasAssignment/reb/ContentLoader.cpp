#include "ContentLoader.h"

namespace reb {

	//loads a .reb file and puts the generated shared pointer to a Content object in the map with the given index
	void ContentLoader::loadContent(LoaderIndex index, std::string fileName) {
		if (m_content.find(index) == m_content.end()) {
			m_content.insert(std::pair<LoaderIndex, Content*>(index, new Content(CONTENTFOLDER + fileName + CONTENTEXTENSION)));
			m_manager.addContent(m_content.at(index));
		}
	};

	//loads a batch of .reb files using an r-value
	void ContentLoader::loadContentBatch(std::vector<std::pair<LoaderIndex, std::string>>&& batch) {
		for (auto& pair : batch) {
			loadContent(pair.first, pair.second);
		}
	}

	//loads a batch of .reb files using an l-value
	void ContentLoader::loadContentBatch(std::vector<std::pair<LoaderIndex, std::string>>& batch) {
		for (auto& pair : batch) {
			loadContent(pair.first, pair.second);
		}
	}

	//returns a shared pointer to a content object at the given index
	Content* ContentLoader::getContent(LoaderIndex index) {
		auto& found = m_content.at(index);
		return found;
	};

	//re-manages the textures into a new spritesheet
	void ContentLoader::manage() {
		m_manager.sort();
	};

	//loads a tilemap
	Tilemap& ContentLoader::loadTilemap(std::string fileLocation, std::vector<std::pair<TileIndex, TileIndex>> tileRanges) {
		m_tilemaps.emplace_back(fileLocation, tileRanges);
		for (auto& content : m_tilemaps.back().m_contents) {
			m_manager.addContent(content.second);
		}
		auto& toReturn = m_tilemaps.back();
		return toReturn;
	};

	//destructor
	ContentLoader::~ContentLoader() {
		for (auto& content : m_content) {
			delete content.second;
		}
	}

}