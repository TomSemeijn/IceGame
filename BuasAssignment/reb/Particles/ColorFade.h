#ifndef REBOOTPARTICLESCOLORFADE_H
#define REBOOTPARTICLESCOLORFADE_H

#pragma once
#include "../ParticleComp.h"

namespace reb {

	namespace Particles {

		//makes the particle fade between colors using its lifetime
		class ColorFade : public ParticleComp {
		private:
			//vector of all the colors
			std::vector<Color> m_colors;
		public:
			//constructor
			ColorFade(std::vector<Color> colors) : m_colors{ colors } {};

			//sets inital color to the first in the vector when the component is added to a particle
			void onOwnerSet()override;

			//updates the component to allow behavior to be added to the owning particle
			virtual void update(float elapsed)override;

			//returns a clone of the component
			virtual ParticleComp* clone()override;

		};

	}

}

#endif