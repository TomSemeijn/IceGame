#ifndef REBOOTCONTENT_H
#define REBOOTCONTENT_H

#pragma once
#include <vector>
#include <mutex>
#include <SFML/Graphics.hpp>
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"

namespace reb {

	//the content class loads the requested file on construction and unserializes it into its data members
	class Content {
	private:
		//the name of the file the content object was constructed from
		std::string m_fileName;

		//an sfml image containing both the diffuse and normal maps
		sf::Image m_diffuseImage;
		sf::Image m_normalImage;
		//the area containing the image on the texture
		Rect<int> m_area;

		//a pointer to the normal texture
		sf::Texture* m_normalTexture = nullptr;

		//an sfml sprite containing the diffuse image
		sf::Sprite m_diffuseSprite;

		//a collider of convex shapes
		std::vector<std::vector<Vector2>> m_convexes;

		//true if the diffuse image contains any not fully opaque pixels
		bool m_hasTransparency;

	public:
		//empty constructor
		Content() {};
		//the constructor which loads and serializes the given file
		Content(std::string fileName);

		Content(const Content&) = delete;

		//const references
		const std::string& getFileName()const { return m_fileName; };
		const sf::Image& getDiffuseImage() { return m_diffuseImage; };
		const sf::Image& getNormalImage() { return m_normalImage; };
		const Rect<int>& getArea() { return m_area; };
		const std::vector<std::vector<Vector2>>& getConvexes() { return m_convexes; };
		const sf::Texture& getNormalTexture() { return *m_normalTexture; };

		//sprite reference
		sf::Sprite& getDiffuseSprite() { return m_diffuseSprite; };

		//sets the origin of the sprite in the texture
		void setSpriteOrigin(int x, int y, sf::Texture& texture);

		//sets the normal texture pointer
		void setNormalPointer(sf::Texture* normal);

		//returns true if the diffuse image contains any not fully opaque pixels
		bool hasTransparency();
	};

}

#endif