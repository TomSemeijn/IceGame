#ifndef REBOOTPARTICLESMOVEDIRSHIFT_H
#define REBOOTPARTICLESMOVEDIRSHIFT_H

#pragma once
#include "../ParticleComp.h"

namespace reb {

	namespace Particles {

		//moves a particle by changing the direction every random amount of seconds by a random amount
		class MoveDirShift : public ParticleComp {
		private:
			//ranges for velocity and direction
			std::pair<float, float> m_initialDirRange;
			std::pair<float, float> m_shiftRange;
			std::pair<float, float> m_shiftTimeRange;
			std::pair<float, float> m_velocityRange;

			//the values used by this component
			float m_currentDir;
			float m_velocity;
			float m_currentShiftTime;
			float m_currentTime = 0;
		public:
			//constructor (directionRange is in degrees)
			MoveDirShift(std::pair<float, float> initialDirRange, std::pair<float, float> shiftRange, std::pair<float, float> shiftTimeRange, std::pair<float, float> velocityRange);

			//updates the component to allow behavior to be added to the owning particle
			virtual void update(float elapsed)override;

			//returns a clone of the component
			virtual ParticleComp* clone()override;
		};

	}

}

#endif