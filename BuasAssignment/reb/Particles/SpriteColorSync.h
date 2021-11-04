#ifndef REBOOTPARTICLESSPRITECOLORSYNC_H
#define REBOOTPARTICLESSPRITECOLORSYNC_H

#pragma once
#include "ObjSync.h"

namespace reb {

	namespace Particles {

		//syncs the color of the particle with the sprite of the particle-managed object
		class SpriteColorSync : public ObjSync {
		protected:
			//tries to get sprite and does intial syncs
			void onObjOwnerSet()override;
		public:
			//syncs if there is a sprite
			void update(float elapsed)override;

			//returns a clone of the component
			ParticleComp* clone()override;
		};

	}

}

#endif