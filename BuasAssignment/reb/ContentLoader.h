#ifndef REBOOTCONTENTLOADER_H
#define REBOOTCONTENTLOADER_H

#pragma once
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <thread>
#include <mutex>
#include "Content.h"
#include "TextureManager.h"
#include "Tilemap.h"

namespace reb {

	extern enum ContentIndex : int;
	extern enum TileIndex : int;

	//the contentLoader class loads, holds, distributes and unloads all content loaded from .reb files
	class ContentLoader {
	public:
		//a variant to include the ContentIndex and TileIndex enums, so tiles dont need a value in both enums
		typedef std::variant<ContentIndex, TileIndex> LoaderIndex;
	private:
		//constants for content file locations
		const std::string CONTENTFOLDER = "Content/";
		const std::string CONTENTEXTENSION = ".reb";

		//map of currently loaded content
		std::map<LoaderIndex, Content*> m_content;

		//vector of tilemaps
		std::vector<Tilemap> m_tilemaps;

		//testuremanager for putting textures into spritesheets
		TextureManager m_manager;
	public:
		ContentLoader() {};
		ContentLoader(const ContentLoader&) = delete;

		//content loaders
		void loadContent(LoaderIndex index, std::string fileName);
		void loadContentBatch(std::vector<std::pair<LoaderIndex, std::string>>&& batch);
		void loadContentBatch(std::vector<std::pair<LoaderIndex, std::string>>& batch);

		//loads a tilemap
		Tilemap& loadTilemap(std::string fileLocation, std::vector<std::pair<TileIndex, TileIndex>> tileRanges);

		//returns a shared pointer to a content object at the given index
		Content* getContent(LoaderIndex index);

		//re-manages the textures into a new spritesheet
		void manage();

		//destructor
		~ContentLoader();
	};

}

#endif