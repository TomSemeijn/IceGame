#ifndef REBOOTPARTICLESTIMEDSCALE_H
#define REBOOTPARTICLESTIMEDSCALE_H

#pragma once
#include "../ParticleComp.h"

namespace reb {

	namespace Particles {

		//uses the given pairs of time-percentage and opacity values to fade the particle on a more complicated track
		class TimedScale : public ParticleComp {
		private:
			//updates the scale of the particle
			void updateScale();
		protected:
			//the track of the fade
			std::vector<std::pair<float, float>> m_track;
		public:
			//constructor
			TimedScale(std::vector<std::pair<float, float>> track);

			//sets inital scale when the component is added to a particle
			void onOwnerSet()override;

			//updates the component to allow behavior to be added to the owning particle
			virtual void update(float elapsed)override;

			//returns a clone of the component
			virtual ParticleComp* clone()override;

		};

	}

}

#endif