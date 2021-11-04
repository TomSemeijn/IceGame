#include "TextureManager.h"

namespace reb {

	//adds a content object to be sorted
	void TextureManager::addContent(Content* content) {
		m_toSort.push_back(content);
	}

	//sorts the images into texture atlasses
	void TextureManager::sort() {

		std::vector<ImageRect> unPositioned{};
		for (auto& sort : m_toSort) {
			unPositioned.emplace_back(0, 0, sort);
		}

		//sorts images by height in descending order
		std::sort(unPositioned.begin(), unPositioned.end(), [](ImageRect& a, ImageRect& b) {
			return a.m_rectangle.height > b.m_rectangle.height;
		});

		auto firstUnpositioned = unPositioned.data();
		auto unpositionedSize = unPositioned.size();

		//gets general information about the images
		int highestWidth = 0;
		int highestHeight = unPositioned.front().m_rectangle.height;
		int totalArea = 0;
		int totalWidth = 0;
		for (int k = 0; k < unpositionedSize; k++) {
			auto img = (firstUnpositioned + k);
			highestWidth = std::max(img->m_rectangle.width, highestWidth);
			totalArea += img->m_rectangle.width * img->m_rectangle.height;
			totalWidth += img->m_rectangle.width;
		}

		//initializes vector of positioned images
		std::vector<ImageRect> finalSheet{};
		int currentBoundArea = 0;
		int currentBoundWidth = 0;
		int currentBoundHeight = 0;
		
		//initializes the size of the bounding rectangle
		unsigned int maxWidth = totalWidth;
		unsigned int maxHeight = highestHeight;

		while (maxWidth > highestWidth) {

			//makes sure the max area is more than or equal to the total area of all the rectangles
			while (maxWidth * maxHeight < totalArea) {
				maxHeight++;
			}

			//skips if the max area is more than the best area so far
			if (maxWidth * maxHeight > currentBoundArea && currentBoundArea > 0) {
				maxWidth--;
				continue;
			}

			//initializes vector for positioned rectangles with the current constraints
			std::vector<ImageRect> positioned{};

			//adds first image at (0, 0)
			positioned.emplace_back(0, 0, unPositioned.front().m_img);

			auto posSize = positioned.size();
			auto firstPos = positioned.data();

			//loops through all images to add
			for (int k = 1; k < unpositionedSize; k++) {

				auto img = (firstUnpositioned + k);

				bool foundPos = false;

				bool intersectsRect = false;
				Rect<int> intersectsWith{};

				//loops through all pixels within the available space
				for (int x = 0; x < maxWidth; x++) {
					for (int y = 0; y < maxHeight; y++) {

						//checks if the rectangle fits with the current position as an origin
						if (y + img->m_rectangle.height > maxHeight) { continue; }
						if (x + img->m_rectangle.width > maxWidth) { continue; }

						//checks if the rectangle intersects with any other rectangle at this origin
						Rect<int> newRect{ x, y, img->m_rectangle.width, img->m_rectangle.height };
						intersectsRect = false;
						for (int k = 0; k < posSize; k++) {
							if (newRect.intersectsRect((firstPos + k)->m_rectangle)) {
								intersectsRect = true;
								y = (firstPos + k)->m_rectangle.getBottom();
								intersectsWith = (firstPos + k)->m_rectangle;
								break;
							}
						}
						
						//skips if any rectangles intersected
						if (intersectsRect) { continue; }

						//adds the rectangle at the current origin
						positioned.emplace_back(x, y, img->m_img);
						foundPos = true;

						posSize = positioned.size();
						firstPos = positioned.data();

						break;
					}
					if (foundPos) { break; }
				}
				if (!foundPos) { break; }
			}

			//if not all rectangles were placed
			if (posSize != unpositionedSize) {
				//increments the max height of the sheet
				maxHeight++;
			}
			//if all rectangles were placed
			else {
				//gets the bounds of the current spritesheet
				int currentWidth = 0;
				int currentHeight = 0;
				for (int k = 0; k < posSize; k++) {
					auto pos = (firstPos + k);
					currentWidth = std::max(currentWidth, pos->m_rectangle.getRight());
					currentHeight = std::max(currentHeight, pos->m_rectangle.getBottom());
				}
				//if the current spritesheet has a lower area than the last
				if (currentWidth * currentHeight < currentBoundArea || !(currentBoundArea > 0)) {
					//stores the current spritesheet
					currentBoundWidth = currentWidth;
					currentBoundHeight = currentHeight;
					currentBoundArea = currentWidth * currentHeight;
					finalSheet = positioned;
				}
				//decrements the max width of the sheet
				maxWidth--;
			}

		}

		//creates the texture
		m_texture.create(currentBoundWidth, currentBoundHeight);
		m_texture.setSrgb(true);

		m_normalTexture = new sf::Texture();
		m_normalTexture->create(currentBoundWidth, currentBoundHeight);
		m_normalTexture->setSrgb(true);
		
		auto posSize = finalSheet.size();
		auto firstPos = finalSheet.data();

		//loops through all positioned sprites
		for (int k = 0; k < posSize; k++) {
			auto pos = (firstPos + k);
			
			//adds sprite to texture
			m_texture.update(pos->m_img->getDiffuseImage(), pos->m_rectangle.left, pos->m_rectangle.top);
			m_normalTexture->update(pos->m_img->getNormalImage(), pos->m_rectangle.left, pos->m_rectangle.top);

			//sets diffusearea in content
			pos->m_img->setSpriteOrigin(pos->m_rectangle.left, pos->m_rectangle.top, m_texture);

			//sets the normal texture
			pos->m_img->setNormalPointer(m_normalTexture);
		}
		
		//saves the resulting spritesheet
		saveResults();
		
	}

	//saves all current texture atlasses to image files for debugging
	void TextureManager::saveResults() {
		m_texture.copyToImage().saveToFile("Content/spritesheet.png");
		m_normalTexture->copyToImage().saveToFile("Content/spritesheet_n.png");
	}

	//destructor
	TextureManager::~TextureManager() {
		//frees memory for the normal texture
		delete m_normalTexture;
	}

}