#ifndef REBOOTPIXELPARTICLE_H
#define REBOOTPIXELPARTICLE_H

#pragma once
#include "Particle.h"

namespace reb {

	//a simple particle that's only a single world-unit-sized square with a color (can still be scaled and rotated of course)
	class PixelParticle : public Particle {
	public:
		//default constructor
		PixelParticle(Color color) : Particle{color} {};
		//the constructor
		PixelParticle(float lifeTime, Vector2 pos, float scale, float rotation, Color color) : Particle{ lifeTime, pos, scale, rotation, color } { };

		//draws the particle to the screen
		void draw(DrawInfo& drawInfo, const sf::FloatRect& viewBox)override;

		//returns a clone of the particle with the given transform info
		Particle* clone(float lifeTime, Vector2 pos, float scale, float rotation)override;
	};

}

#endif