#ifndef REBOOTTEXTUREMANAGER_H
#define REBOOTTEXTUREMANAGER_H

#pragma once
#include <SFML/Graphics.hpp>
#include "Rect.h"
#include "Content.h"

namespace reb {

	//sorts the given content objects into a spritesheet and changes the textures used to that spritesheet as well
	class TextureManager {
	private:
		//the content objects to sort
		std::vector<Content*> m_toSort{};
		
		//the resulting texture
		sf::Texture m_texture{};
		//the resulting normal texture (a pointer because it needs to be given to the contnet objects seperately)
		sf::Texture* m_normalTexture;

		//a structure to keep track of which content objects occupy what space during the sorting process
		struct ImageRect {
			Rect<int> m_rectangle;
			Content* m_img;
			ImageRect(int x, int y, Content* img)
				: m_rectangle{ x, y, img->getArea().width + 1, img->getArea().height + 1 }, m_img{ img }
			{};
		};

	public:
		//adds a content object to sort
		void addContent(Content* content);

		//sorts the contents
		void sort();

		//saves the spritesheet to a file (for debugging)
		void saveResults();

		//destructor
		~TextureManager();
	};

}

#endif