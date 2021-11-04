#include "DestroyOutBounds.h"

//constructor
DestroyOutBounds::DestroyOutBounds(const reb::Rect<float> bounds) : m_bounds{ bounds } {};

//despawns the object if it's gone completely out of bounds (using the bounds from the transform)
void DestroyOutBounds::update(float elapsed) {
	if (hasOwner()) {
		reb::Transform* transf = getOwner()->getTransform();
		reb::Rect<float> objBounds = transf->getBounds();
		if (
			!m_bounds.withinRect(transf->getWorldSpace(reb::Vector2(objBounds.left, objBounds.top)))
			&& !m_bounds.withinRect(transf->getWorldSpace(reb::Vector2(objBounds.left + objBounds.width, objBounds.top)))
			&& !m_bounds.withinRect(transf->getWorldSpace(reb::Vector2(objBounds.left + objBounds.width, objBounds.top + objBounds.height)))
			&& !m_bounds.withinRect(transf->getWorldSpace(reb::Vector2(objBounds.left, objBounds.top + objBounds.height)))
			) {
			getOwner()->destroy();
		}
	}
}

//returns a copy of the component
DestroyOutBounds* DestroyOutBounds::clone()const { return new DestroyOutBounds(*this); };