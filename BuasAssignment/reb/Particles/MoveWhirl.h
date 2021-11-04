#ifndef REBOOTPARTICLESMOVEWHIRL_H
#define REBOOTPARTICLESMOVEWHIRL_H

#pragma once
#include "../ParticleComp.h"

namespace reb {

	namespace Particles {

		//moves particles in a whirlpool pattern by moving in a circle with an expanding radius
		class MoveWhirl : public ParticleComp {

		private:
			//ranges for velocity and direction
			std::pair<float, float> m_radiusExpansionRange;
			std::pair<float, float> m_velocityRange;

			//the actual values used by this component
			float m_expansion;
			float m_radius = 0;
			float m_velocity;

			//the initial position of the particle
			Vector2 m_initialPos;
		protected:
			//sets the initial position when the owning particle is set
			virtual void onOwnerSet()override;

		public:
			//constructor (directionRange is in degrees)
			MoveWhirl(std::pair<float, float> radiusExpansionRange, std::pair<float, float> velocityRange);

			//updates the component to allow behavior to be added to the owning particle
			virtual void update(float elapsed)override;

			//returns a clone of the component
			virtual ParticleComp* clone()override;

		};

	}

}

#endif