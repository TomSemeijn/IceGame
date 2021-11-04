#include "ObjScaleSync.h"

//syncs on update
void ObjScaleSync::update(float elapsed) {
	if (hasOwner() && hasObjOwner()) {
		getObjOwner()->getObject()->getTransform()->setScale(getOwner()->m_transform.getScale());
	}
}

//clones the component
ObjScaleSync* ObjScaleSync::clone() {
	return new ObjScaleSync();
};