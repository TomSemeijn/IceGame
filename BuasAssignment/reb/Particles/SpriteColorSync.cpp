#include "SpriteColorSync.h"
#include "../Drawable.h"

namespace reb {

	namespace Particles {

		//returns a clone of the component
		ParticleComp* SpriteColorSync::clone() {
			return new SpriteColorSync(*this);
		}

		//tries to get sprite and does intial syncs
		void SpriteColorSync::onObjOwnerSet() {
			//gets owner
			auto objOwner = getObjOwner();
			//tries to get object
			if (auto obj = objOwner->getObject()) {
				//tries to get sprite
				if (auto spr = obj->getSprite()) {
					//syncs the color
					spr->setColor(objOwner->m_color);
				}
			}
		}

		//syncs if there is a sprite
		void SpriteColorSync::update(float elapsed) {
			//verifies there is an ObjParticle owner
			if (hasObjOwner()) {
				//gets owner
				auto objOwner = getObjOwner();
				//tries to get object
				if (auto obj = objOwner->getObject()) {
					//tries to get sprite
					if (auto spr = obj->getSprite()) {
						//syncs the color
						spr->setColor(objOwner->m_color);
					}
				}
			}
		}

	}

}