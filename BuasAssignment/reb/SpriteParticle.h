#ifndef REBOOTSPRITEPARTICLE_H
#define REBOOTSPRITEPARTICLE_H

#pragma once
#include "Particle.h"
#include "Content.h"

namespace reb {

	//a particle that's a sprite
	class SpriteParticle : public Particle {
	protected:
		//the content the particle takes its sprite from
		Content* m_source;

	public:
		//default constructor
		SpriteParticle(Content* source, Color color = Color{ 255, 255, 255 }) : Particle{color}, m_source{ source } {};
		//the constructor
		SpriteParticle(float lifeTime, Vector2 pos, float scale, float rotation, Content* source, Color color = Color(255, 255, 255)) : Particle{ lifeTime, pos, scale, rotation, color }, m_source{ source } { };

		//draws the particle to the screen
		void draw(DrawInfo& drawInfo, const sf::FloatRect& viewBox)override;

		//returns a clone of the particle with the given transform info
		virtual Particle* clone(float lifeTime, Vector2 pos, float scale, float rotation)override;

	};

}

#endif