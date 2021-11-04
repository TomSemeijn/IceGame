#include "ObjSync.h"

namespace reb {

	namespace Particles {

		//checks if the set owner is an object particle
		void ObjSync::onOwnerSet() {
			if (ObjParticle* objOwner = dynamic_cast<ObjParticle*>(getOwner())) {
				m_objectOwner = objOwner;
				onObjOwnerSet();
			}
			else {
				m_objectOwner = nullptr;
			}
		}

		//returns true if there is an object particle owner
		bool ObjSync::hasObjOwner() { return m_objectOwner != nullptr; };

		//returns the object particle owner if there is one
		ObjParticle* ObjSync::getObjOwner() { return m_objectOwner; };

	}

}