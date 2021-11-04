#include "Content.h"
#include <rapidxml/rapidxml_utils.hpp>
#include "Color.h"
#include "Vector3.h"

namespace reb {

	//the constructor which loads and serializes the given file
	Content::Content(std::string fileName) : m_fileName{ fileName }, m_hasTransparency{ false } {
		//gets the xml file
		rapidxml::file<> file(fileName.data());
		//parses the xml file
		rapidxml::xml_document<> doc;
		doc.parse<0>(file.data());

		//adds the diffuse texture
		{
			rapidxml::xml_node<> *mainNode = doc.first_node()->first_node("d");
			//gets the width and height
			unsigned int width = std::atoi(mainNode->first_attribute("w")->value());
			unsigned int height = std::atoi(mainNode->first_attribute("h")->value());
			m_diffuseImage.create(width, height);
			//sets up rectangle information
			m_area = Rect<int>(0, 0, width, height);
			//goes through all the pixels
			rapidxml::xml_node<> *pixelNode = mainNode->first_node();
			unsigned int x = 0;
			unsigned int y = 0;
			do {
				//sets the current values
				sf::Uint8 r{ (sf::Uint8)std::atoi(pixelNode->first_attribute("r")->value()) };
				sf::Uint8 g{ (sf::Uint8)std::atoi(pixelNode->first_attribute("g")->value()) };
				sf::Uint8 b{ (sf::Uint8)std::atoi(pixelNode->first_attribute("b")->value()) };
				sf::Uint8 a{ (sf::Uint8)std::atoi(pixelNode->first_attribute("a")->value()) };
				if (!m_hasTransparency && a < 255 && a > 0 && (r > 0 || g > 0 || b > 0)) { m_hasTransparency = true; }
				//sets the pixel in the image
				m_diffuseImage.setPixel(x, y, sf::Color(r, g, b, a));
				//increments x and y
				if (x + 1 < width) { x++; }
				else { x = 0; y++; }
			} while (pixelNode = pixelNode->next_sibling());
		}

		//adds the normals
		{
			rapidxml::xml_node<> *mainNode = doc.first_node()->first_node("n");
			//gets the width and height
			unsigned int width = std::atoi(mainNode->first_attribute("w")->value());
			unsigned int height = std::atoi(mainNode->first_attribute("h")->value());
			m_normalImage.create(width, height);
			//goes through all the pixels
			rapidxml::xml_node<> *pixelNode = mainNode->first_node();
			unsigned int x = 0;
			unsigned int y = 0;
			do {
				//sets the current values
				sf::Uint8 r{ (sf::Uint8)std::atoi(pixelNode->first_attribute("r")->value()) };
				sf::Uint8 g{ (sf::Uint8)std::atoi(pixelNode->first_attribute("g")->value()) };
				sf::Uint8 b{ (sf::Uint8)std::atoi(pixelNode->first_attribute("b")->value()) };
				sf::Uint8 a{ (sf::Uint8)std::atoi(pixelNode->first_attribute("a")->value()) };
				//sets the pixel in the image
				m_normalImage.setPixel(x, y, sf::Color(r, g, b, a));
				//increments x and y
				if (x + 1 < width) { x++; }
				else { x = 0; y++; }
			} while (pixelNode = pixelNode->next_sibling());
		}

		//adds the collider
		{
			rapidxml::xml_node<> *mainNode = doc.first_node()->first_node("c");
			//goes through all the convexes
			rapidxml::xml_node<> *convexNode = mainNode->first_node();
			if (convexNode != nullptr) {
				do {
					//goes through all the vertices
					rapidxml::xml_node<>* vertexNode = convexNode->first_node();
					if (vertexNode != nullptr) {
						std::vector<Vector2> vertices{};
						do {
							//adds the vertex
							vertices.emplace_back(std::atof(vertexNode->first_attribute("x")->value()), std::atof(vertexNode->first_attribute("y")->value()));
						} while (vertexNode = vertexNode->next_sibling());
						m_convexes.push_back(vertices);
					}
				} while (convexNode = convexNode->next_sibling());
			}
		}

		//clears the rapidxml memory
		doc.clear();

	}

	//sets the origin of the sprite in the texture
	void Content::setSpriteOrigin(int x, int y, sf::Texture& texture) {
		m_area.left = x;
		m_area.top = y;
		m_diffuseSprite = sf::Sprite(texture, sf::IntRect(x, y, m_area.width, m_area.height));
	}

	//sets the normal texture pointer
	void Content::setNormalPointer(sf::Texture* normal) {
		m_normalTexture = normal;
	}

	//returns true if the diffuse image contains any not fully opaque pixels
	bool Content::hasTransparency() {
		return m_hasTransparency;
	}

}