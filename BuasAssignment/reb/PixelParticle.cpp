#include "PixelParticle.h"

namespace reb {
	
	//draws the particle to the screen
	void PixelParticle::draw(DrawInfo& drawInfo, const sf::FloatRect& viewBox) {
		//creates the sfml rectangleshape
		sf::RectangleShape drawShape{ sf::Vector2f{1, 1} };
		drawShape.setOrigin(sf::Vector2f{ 0.5, 0.5 });
		drawShape.setScale(sf::Vector2f{ m_transform.getScale(), m_transform.getScale() });
		drawShape.setRotation(getDegrees(m_transform.getRotation()));
		drawShape.setPosition(m_transform.getPos().getSfml());
		//gets the particle corners
		auto trans = drawShape.getTransform();
		auto local = drawShape.getLocalBounds();
		sf::Vector2f topLeft = trans * sf::Vector2f(local.left, local.top);
		sf::Vector2f topRight = trans * sf::Vector2f(local.left + local.width, local.top);
		sf::Vector2f bottomLeft = trans * sf::Vector2f(local.left, local.top + local.height);
		sf::Vector2f bottomRight = trans * sf::Vector2(local.left + local.width, local.top + local.height);
		//adds the vertices
		drawInfo.m_verts.emplace_back(topLeft, m_color.getSfml());
		drawInfo.m_verts.emplace_back(topRight, m_color.getSfml());
		drawInfo.m_verts.emplace_back(bottomRight, m_color.getSfml());
		drawInfo.m_verts.emplace_back(bottomLeft, m_color.getSfml());
	}

	//returns a clone of the particle with the given transform info
	Particle* PixelParticle::clone(float lifeTime, Vector2 pos, float scale, float rotation) {
		PixelParticle* newParticle = new PixelParticle(lifeTime, pos, scale, rotation, ORIGINALCOLOR);
		newParticle->cloneComponentsFrom(this);
		return newParticle;
	}

}