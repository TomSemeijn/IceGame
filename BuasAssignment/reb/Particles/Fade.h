#ifndef REBOOTPARTICLESFADE_H
#define REBOOTPARTICLESFADE_H

#pragma once
#include "../ParticleComp.h"

namespace reb {

	namespace Particles {

		//makes a particle fade in and out using its lifetime
		class Fade : public ParticleComp {
		public:
			//sets inital opcacity to zero when the component is added to a particle
			void onOwnerSet()override;

			//updates the component to allow behavior to be added to the owning particle
			virtual void update(float elapsed)override;

			//returns a clone of the component
			virtual ParticleComp* clone()override;

		};

	}

}

#endif