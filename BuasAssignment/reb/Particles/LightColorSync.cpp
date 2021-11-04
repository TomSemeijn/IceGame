#include "LightColorSync.h"
#include "../LightSource.h"

namespace reb {

	namespace Particles {
		
		//returns a clone of the component
		ParticleComp* LightColorSync::clone() {
			return new LightColorSync(*this);
		}

		//sets the initial values when an owner is set
		void LightColorSync::onObjOwnerSet() {
			//gets the new owner
			auto objOwner = getObjOwner();

			//tries to get object
			if (auto obj = objOwner->getObject()) {
				//tries to get lightsource
				if (auto light = obj->getLight()) {

					//sets hadLightsource and the original intensity
					m_hadLightsource = true;
					m_originalIntensity = light->getIntensity();

					//syncs the color and intensity
					light->setColor(objOwner->m_color);
					light->setIntensity(m_originalIntensity * ((float)objOwner->m_color.getA() / 255.0f));

				}
				//sets hasLightsource to false if there was none
				else {
					m_hadLightsource = false;
				}
			}
			
		}

		//syncs the color to the lightsource
		void LightColorSync::update(float elapsed) {
			//verifies there is an ObjParticle owner
			if (hasObjOwner()) {
				//gets the owner
				auto objOwner = getObjOwner();

				//tries to get the object
				if (auto obj = objOwner->getObject()) {
					//tries to get light
					if (auto light = objOwner->getObject()->getLight()) {
						//gets initial intensity if the lightsource wasnt there when the owner was set
						if (!m_hadLightsource) {
							m_hadLightsource = true;
							m_originalIntensity = light->getIntensity();
						}
						//syncs the color and intensity
						light->setColor(objOwner->m_color);
						light->setIntensity(m_originalIntensity * ((float)objOwner->m_color.getA() / 255.0f));
					}
				}
				
			}
		}

	}

}