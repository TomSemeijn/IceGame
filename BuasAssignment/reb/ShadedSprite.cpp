#include "ShadedSprite.h"
#include "GameObj.h"
#include "Renderer.h"

namespace reb {

	//returns a copy of the component
	ShadedSprite* ShadedSprite::clone()const {
		return new ShadedSprite{ *this };
	}

	//draws the sprite on the referenced window using the referenced transform for the location
	void ShadedSprite::draw(Renderer& renderer, const sf::FloatRect& viewBox, const sf::Vector2u& windowSize) {

		//verifies the sprite has an owner
		if (hasOwner()) {
			//gets the transform of the owning object
			Transform*const transform = getOwner()->getTransform();
			//sets the origin of the sprite to the center of the transform
			Vector2 center = transform->getCenter();
			center.compProduct(Vector2(windowSize.x, windowSize.y));
			sf::Sprite& diffuseSprite = m_source->getDiffuseSprite();
			if (ROUNDMODE == RoundMode::MODE_FLOOR) {
				center = center.getFloored();
			}
			else if (ROUNDMODE == RoundMode::MODE_ROUND) {
				center.x = std::roundf(center.x);
				center.y = std::roundf(center.y);
			}
			else if (ROUNDMODE == RoundMode::MODE_CEIL) {
				center.x = std::ceilf(center.x);
				center.y = std::ceilf(center.y);
			}
			center.compDiv(Vector2(windowSize.x, windowSize.y));
			diffuseSprite.setOrigin(center.getSfml());
			//sets the position of the sprite to the position of the transform (plus the center because the origin was changed)
			Vector2 pos = transform->getWorldSpace(center + m_offset);
			if (ROUNDMODE == RoundMode::MODE_FLOOR) {
				pos = pos.getFloored();
			}
			else if (ROUNDMODE == RoundMode::MODE_ROUND) {
				pos.x = std::roundf(pos.x);
				pos.y = std::roundf(pos.y);
			}
			else if (ROUNDMODE == RoundMode::MODE_CEIL) {
				pos.x = std::ceilf(pos.x);
				pos.y = std::ceilf(pos.y);
			}
			diffuseSprite.setPosition(pos.getSfml());
			//sets the rotation of the sprite to that of the transform converted to degrees
			diffuseSprite.setRotation(transform->getRotation() * (180 / PI));
			//scales the sprite
			diffuseSprite.setScale(sf::Vector2f(transform->getScale(), transform->getScale()));
			//flips the sprite
			diffuseSprite.scale(sf::Vector2f((getOwner()->getTransform()->getFlippedX() ? -1 : 1), (getOwner()->getTransform()->getFlippedY() ? -1 : 1)));
			//sets the opacity of the sprite
			diffuseSprite.setColor(m_color.getSfml());

			//gets the global bounds of the sprite
			auto bounds = diffuseSprite.getGlobalBounds();

			//only draws when within world area
			if (bounds.intersects(viewBox)) {
				//draws the sprite on the window
				renderer.draw(diffuseSprite, m_source, m_alpha, m_cells, transform->getZ());
			}
		}

	}

	//gets the alpha value
	float ShadedSprite::getAlpha() { return m_alpha; }

	//sets the alpha value
	void ShadedSprite::setAlpha(float newAlpha) { m_alpha = newAlpha; }

	//gets the cell shading level
	int ShadedSprite::getCellShadingLevel() { return m_cells; }

	//sets the cell shading level
	void ShadedSprite::setCellShadingLevel(int newCell) { m_cells = newCell; }

}