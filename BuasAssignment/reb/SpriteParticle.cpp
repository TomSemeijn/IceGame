#include "SpriteParticle.h"

namespace reb {

	//draws the particle to the screen
	void SpriteParticle::draw(DrawInfo& drawInfo, const sf::FloatRect& viewBox) {
		//gets the center of the sprite
		sf::Vector2f center = m_source->getArea().getCenter().getSfml();
		sf::Sprite& diffuseSprite = m_source->getDiffuseSprite();
		diffuseSprite.setOrigin(center);
		//sets the position of the sprite to the position of the transform (plus the center because the origin was changed)
		Vector2 pos = m_transform.getWorldSpace(center);
		diffuseSprite.setPosition(pos.getSfml());
		//sets the rotation of the sprite to that of the transform converted to degrees
		diffuseSprite.setRotation(m_transform.getRotation() * (180 / PI));
		//scales the sprite
		diffuseSprite.setScale(sf::Vector2f(m_transform.getScale(), m_transform.getScale()));
		//flips the sprite
		diffuseSprite.scale(sf::Vector2f((m_transform.getFlippedX() ? -1 : 1), (m_transform.getFlippedY() ? -1 : 1)));

		//gets the global bounds of the sprite
		auto bounds = diffuseSprite.getGlobalBounds();

		//only draws when within world area
		if (bounds.intersects(viewBox)) {
			//gets the sprite corners
			auto trans = diffuseSprite.getTransform();
			auto local = diffuseSprite.getLocalBounds();
			sf::Vector2f topLeft = trans * sf::Vector2f(local.left, local.top);
			sf::Vector2f topRight = trans * sf::Vector2f(local.left + local.width, local.top);
			sf::Vector2f bottomLeft = trans * sf::Vector2f(local.left, local.top + local.height);
			sf::Vector2f bottomRight = trans * sf::Vector2(local.left + local.width, local.top + local.height);

			//adds the vertices to the drawinfo
			auto texCoords = diffuseSprite.getTextureRect();
			drawInfo.m_verts.emplace_back(topLeft, m_color.getSfml(), sf::Vector2f(texCoords.left, texCoords.top));
			drawInfo.m_verts.emplace_back(topRight, m_color.getSfml(), sf::Vector2f(texCoords.left + texCoords.width, texCoords.top));
			drawInfo.m_verts.emplace_back(bottomRight, m_color.getSfml(), sf::Vector2f(texCoords.left + texCoords.width, texCoords.top + texCoords.height));
			drawInfo.m_verts.emplace_back(bottomLeft, m_color.getSfml(), sf::Vector2f(texCoords.left, texCoords.top + texCoords.height));

			//sets the textures in the drawinfo
			drawInfo.m_diffuseTexture = const_cast<sf::Texture*>(diffuseSprite.getTexture());
			drawInfo.m_normalTexture = const_cast<sf::Texture*>(&m_source->getNormalTexture());
		}
	}

	//returns a clone of the particle with the given transform info
	Particle* SpriteParticle::clone(float lifeTime, Vector2 pos, float scale, float rotation) {
		SpriteParticle* newPart = new SpriteParticle(lifeTime, pos, scale, rotation, m_source, ORIGINALCOLOR);
		newPart->cloneComponentsFrom(this);
		return newPart;
	}

}