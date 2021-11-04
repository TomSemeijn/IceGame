#include "SpriteComp.h"

namespace reb {

	namespace Views {

		//draws the sprite to the screen
		void SpriteComp::draw(sf::RenderTarget& window, Shader& shader, float scale)const {
			if (hasOwner() && m_sprite != nullptr) {
				//gets sprite
				sf::Sprite& sprite = m_sprite->getDiffuseSprite();
				Rect<float> realArea = getScaledArea(scale);
				//sets position
				sprite.setPosition(sf::Vector2f(realArea.left, realArea.top));
				//does scaling
				sprite.setScale(sf::Vector2f(1, 1));
				auto globalBounds = sprite.getGlobalBounds();
				if (globalBounds.width > 0 && globalBounds.height > 0) {
					switch (m_scaleType) {
					case SpriteScaleType::SPRITE_LETTERBOX:
						{
							//gets the size that would be used if the width would be filled
							float xDivY = globalBounds.width / globalBounds.height;
							Vector2 fullWidth(realArea.width, realArea.width / (xDivY));

							//checks if this size fits
							if (fullWidth.y <= realArea.height) {
								//scales the sprite
								sprite.setScale((fullWidth.compDiv(Vector2(globalBounds.width, globalBounds.height)).getSfml()));
								//centers sprite vertically
								auto newBounds = sprite.getGlobalBounds();
								sprite.move(sf::Vector2f(0, (realArea.height - newBounds.height) / 2));
							}
							//uses a size with filled height if it doesnt fit
							else {
								//gets the size that would be used if the height would be filled
								Vector2 fullHeight(realArea.height * xDivY, realArea.height);
								//scales the sprite
								sprite.setScale((fullHeight.compDiv(Vector2(globalBounds.width, globalBounds.height)).getSfml()));
								//centers sprite horizontalle
								auto newBounds = sprite.getGlobalBounds();
								sprite.move(sf::Vector2f(((realArea.width - newBounds.width) / 2), 0));
							}
						}
						break;
					case SpriteScaleType::SPRITE_STRETCH:
						sprite.setScale(sf::Vector2f(realArea.width / globalBounds.width, realArea.height / globalBounds.height));
						break;
					}
				}
				//sets rotation to zero just in case
				sprite.setRotation(0);
				//sets the color
				sprite.setColor(m_color.getSfml());
				//draws sprite
				window.draw(sprite);
			}
		}

		//getters
		Content*const SpriteComp::getSprite()const {
			return m_sprite;
		}

		Color SpriteComp::getColor()const {
			return m_color;
		}

		SpriteComp::SpriteScaleType SpriteComp::getScaleType()const {
			return m_scaleType;
		}

		//setters
		void SpriteComp::setSprite(Content* newContent) {
			m_sprite = newContent;
		}

		void SpriteComp::setColor(Color newColor) {
			m_color = newColor;
		}

		void SpriteComp::setScaleType(SpriteScaleType newType) {
			m_scaleType = newType;
		}

	}

}