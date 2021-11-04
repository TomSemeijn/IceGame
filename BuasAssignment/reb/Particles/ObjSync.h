#ifndef REBOOTPARTICLESOBJSYNC_H
#define REBOOTPARTICLESOBJSYNC_H

#pragma once
#include "../ParticleComp.h"
#include "../objParticle.h"

namespace reb {

	namespace Particles {

		//abstract base class of particle components used to sync particle information to particle-managed objects
		class ObjSync : public ParticleComp {
			friend BasicParticleObj;
		private:
			//the owner cast as an object particle
			ObjParticle* m_objectOwner = nullptr;
		protected:
			//checks if the set owner is an object particle
			void onOwnerSet()override;

			//called when an object particle is set as an owner
			virtual void onObjOwnerSet() {};
		public:
			//returns true if there is an object particle owner
			bool hasObjOwner();

			//returns the object particle owner if there is one
			ObjParticle* getObjOwner();
		};

	}

}

#endif