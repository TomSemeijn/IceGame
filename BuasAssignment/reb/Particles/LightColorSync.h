#ifndef REBOOTPARTICLESLIGHTCOLORSYNC_H
#define REBOOTPARTICLESLIGHTCOLORSYNC_H

#pragma once
#include "ObjSync.h"

namespace reb {

	namespace Particles {

		//syncs the color of the particle to the color of the lightsource of the object
		class LightColorSync : public ObjSync {
		private:
			//original intensity of the lightsource
			float m_originalIntensity;

			//true if the current object particle has a lightsource
			bool m_hadLightsource = false;
		protected:
			//sets the initial values when an owner is set
			void onObjOwnerSet()override;
		public:
			//syncs the color to the lightsource
			void update(float elapsed)override;

			//returns a clone of the component
			ParticleComp* clone()override;
		};

	}

}

#endif