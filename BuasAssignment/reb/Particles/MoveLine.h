#ifndef REBOOTPARTICLESMOVELINE_H
#define REBOOTPARTICLESMOVELINE_H

#pragma once
#include "../ParticleComp.h"

namespace reb {

	namespace Particles {

		//makes a particle move in a straight line using the given range of velocities and directions
		class MoveLine : public ParticleComp {
		private:
			//ranges for velocity and direction
			std::pair<float, float> m_directionRange;
			std::pair<float, float> m_velocityRange;

			//the actual move vector used by this component
			Vector2 m_moveVec;
		public:
			//constructor (directionRange is in degrees)
			MoveLine(std::pair<float, float> directionRange, std::pair<float, float> velocityRange);

			//returns a local aabb of the component (returns an empty aabb by default)
			virtual Rect<float> getLocalAABB(float maxLifetime)override;

			//updates the component to allow behavior to be added to the owning particle
			virtual void update(float elapsed)override;

			//returns a clone of the component
			virtual ParticleComp* clone()override;

		};

	}

}

#endif