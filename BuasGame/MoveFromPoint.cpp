#include "MoveFromPoint.h"

//constructor
MoveFromPoint::MoveFromPoint(reb::Vector2 point, std::pair<float, float> velocityRange)
	: 
	m_point{ point }, 
	m_velocityRange{ velocityRange }
{};


//despawns the owner when it goes out of bounds
void MoveFromPoint::update(float elapsed) {
	if (hasOwner()) {
		if (!m_moveVecSet) {
			m_moveVecSet = true;
			m_moveVec = (getOwner()->m_transform.getPos() - m_point).setLength(1) * reb::genRandf(m_velocityRange.first, m_velocityRange.second);
		}
		getOwner()->m_transform.move(m_moveVec * elapsed);
	}
};

//returns a clone of the component
MoveFromPoint* MoveFromPoint::clone() {
	return new MoveFromPoint(m_point, m_velocityRange);
};

//returns a local aabb of the component (returns an empty aabb by default)
reb::Rect<float> MoveFromPoint::getLocalAABB(float maxLifetime) {
	reb::Vector2 localPoint = getOwner()->m_transform.getLocalSpace(m_point);
	float highestSpeed = std::max(m_velocityRange.first, m_velocityRange.second);
	float highestDistance = highestSpeed * maxLifetime;
	return reb::Rect<float>{localPoint.x - highestDistance, localPoint.y - highestDistance, highestDistance * 2, highestDistance * 2};
}