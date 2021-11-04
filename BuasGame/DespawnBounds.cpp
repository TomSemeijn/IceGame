#include "DespawnBounds.h"

//despawns the owner when it goes out of bounds
void DespawnBounds::update(float elapsed) {
	if (hasOwner()) {
		auto owner = getOwner();
		if (owner->hasOwner()) {
			reb::Vector2 pos = owner->m_transform.getPos();
			if (!m_bounds.withinRect(pos)) {
				owner->getOwner()->despawn(owner);
			}
		}
	}
};

//returns a clone of the component
DespawnBounds* DespawnBounds::clone() {
	return new DespawnBounds(*this);
};