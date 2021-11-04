#include "TimedScale.h"

namespace reb {

	namespace Particles {

		//constructor
		TimedScale::TimedScale(std::vector<std::pair<float, float>> track) : m_track{ track } {
			//gets the sum of all time values
			float totalTime = 0;
			for (auto& pair : track) { totalTime = std::max(totalTime, pair.first); }
			//sets the time values to percentage values
			for (auto& pair : m_track) { pair.first /= totalTime; }
		}

		//updates the scale of the particle
		void TimedScale::updateScale() {
			//verifies there is an owner
			if (hasOwner()) {
				//gets owner
				auto owner = getOwner();
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
						//gets the new scale value
						float newScale = lastValue * (1.0f - relativeTime) + relativeTime * iter->second;
						//sets the scale
						owner->m_transform.setScale(newScale);
						break;
					}

				}
			}
		}

		//sets inital opcacity when the component is added to a particle
		void TimedScale::onOwnerSet() {
			updateScale();
		}

		//updates the component to allow behavior to be added to the owning particle
		void TimedScale::update(float elapsed) {
			updateScale();
		}

		//returns a clone of the component
		ParticleComp* TimedScale::clone() {
			return new TimedScale(*this);
		}

	}

}