#include "Fade.h"

namespace reb {

	namespace Particles {

		//sets inital opcacity to zero when the component is added to a particle
		void Fade::onOwnerSet() {
			getOwner()->m_color.setA(0);
		}

		//updates the component to allow behavior to be added to the owning particle
		void Fade::update(float elapsed) {
			//verifies there is an owner
			if (hasOwner()) {
				//gets the middle of the lifetime of the particle
				float middleTime = getOwner()->LIFETIME * 0.5f;
				//gets the current time of the particle
				float time = getOwner()->getTime();

				//gets the distance time between the current time and the half time
				float distance = std::fabsf(time - middleTime);

				//sets the opacity
				getOwner()->m_color.setA(255 * (1 - (distance / middleTime)));
			}
		}

		//returns a clone of the component
		ParticleComp* Fade::clone() {
			return new Fade();
		}

	}

}