#include "ReversePosSync.h"

//syncs on update
void ReversePosSync::update(float elapsed) {
	if (hasOwner() && hasObjOwner()) {
		getOwner()->m_transform.setPos(getObjOwner()->getObject()->getTransform()->getPos());
	}
}

//clones the component
ReversePosSync* ReversePosSync::clone() {
	return new ReversePosSync();
};