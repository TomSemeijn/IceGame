#include "OpacityScaleSync.h"

//syncs on update
void OpacityScaleSync::update(float elapsed) {
	if (hasOwner()) {
		getOwner()->m_transform.setScale((float)getOwner()->m_color.getA() / 255.0f);
	}
}

//clones the component
OpacityScaleSync* OpacityScaleSync::clone() {
	return new OpacityScaleSync();
};