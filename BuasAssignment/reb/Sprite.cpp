#include "Sprite.h"
#include "Functions.h"
#include "GameObj.h"
#include "Renderer.h"

namespace reb {

	//returns a copy of the component
	Sprite* Sprite::clone()const {
		return new Sprite{ *this };
	}

	//gets called when the owner of the component is set
	void Sprite::onOwnerSet() {
		updateBounds();
	}

	//updates the content the sprite is based on
	void Sprite::setContent(Content* newContent) {
		//sets the content
		m_source = newContent;
		//tries to update bounds
		updateBounds();
	}

	const Content* Sprite::getContent() {
		return m_source;
	}

	//updates the bounds of the owner if that's the job of the sprite
	void Sprite::updateBounds() {
		//checks if the sprite should update the object bounds
		if (getOwner()->BOUNDTYPE == GameObj::BOUNDS_SPRITE) {
			//sets the bounds
			getOwner()->getTransform()->setBounds(
				Rect<float>(
					m_offset.x, 
					m_offset.y,
					m_source->getArea().width,
					m_source->getArea().height
				)
			);
		}
	}

	//draws the sprite on the referenced window
	void Sprite::draw(Renderer& renderer, const sf::FloatRect& viewBox, const sf::Vector2u& windowSize) {

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
				renderer.draw(diffuseSprite, m_source);
			}
		}

	}

	//removes the sprite from its owning object
	void Sprite::destroy() {
		if (hasOwner()) {
			getOwner()->clearSprite();
		}
	}

}