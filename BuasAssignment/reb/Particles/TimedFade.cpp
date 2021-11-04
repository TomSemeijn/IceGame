#include "TimedFade.h"

namespace reb {

	namespace Particles {

		//constructor
		TimedFade::TimedFade(std::vector<std::pair<float, float>> track) : m_track{ track } {
			//gets the sum of all time values
			float totalTime = 0;
			for (auto& pair : track) { totalTime = std::max(totalTime, pair.first); }
			//sets the time values to percentage values
			for (auto& pair : m_track) { pair.first /= totalTime; }
		}

		//updates the opacity of the particle
		void TimedFade::updateOpacity() {
			//verifies there is an owner
			if (hasOwner()) {
				//gets owner
				auto owner = getOwner();
				if (owner->LIFETIME <= 0) { return; }
				//gets a [0 => 1] value for how far the particle is into its lifecycle
				float lifeFactor = owner->getTime() / owner->LIFETIME;

				//loops through the track to find between which values the opacity should be
				for (auto iter = m_track.begin(); iter < m_track.end(); iter++) {

					//if this is the currently aimed at value
					if (lifeFactor <= iter->first) {
						//gets the last value
						float lastValue = (iter == m_track.begin() ? 0 : (iter - 1)->second);
						//gets the time point relative to these two values
						float lastTime = (iter == m_track.begin() ? 0 : (iter - 1)->first);
						float relativeTime = (lifeFactor - lastTime) / (iter->first - lastTime);
						//gets the new opacity value
						float newOpacity = lastValue * (1.0f - relativeTime) + relativeTime * iter->second;
						//sets the opacity
						owner->m_color.setA((int)(255.0f * newOpacity));
						break;
					}

				}
			}
		}

		//sets inital opcacity when the component is added to a particle
		void TimedFade::onOwnerSet() {
			updateOpacity();
		}

		//updates the component to allow behavior to be added to the owning particle
		void TimedFade::update(float elapsed) {
			updateOpacity();
		}

		//returns a clone of the component
		ParticleComp* TimedFade::clone() {
			return new TimedFade(*this);
		}

	}

}