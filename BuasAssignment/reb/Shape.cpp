#include "Shape.h"
#include "Functions.h"

namespace reb {

	//returns the total mass of the shape using the density of the given material
	float Shape::getMass(Material& mat, float scale)const {
		return mat.density * area * scale;
	}

	//returns the center of mass
	Vector2 Shape::getCOM()const {
		return COM;
	}

	//returns the inertia of the shape given the material
	float Shape::getIntertia(Material& mat, float scale) {
		return getLocalInertia(mat) * scale;
	}

	//returns the minimal enclosing AABB
	Rect<float> Shape::getAABB(Transform& trans)const {
		if(!m_initalizedAABB || !(trans == m_lastAABBTransform)) {
			m_lastAABBTransform = trans;
			m_lastAABB = getCurrentAABB(trans);
			m_initalizedAABB = true;
		}
		return m_lastAABB;
	}

}