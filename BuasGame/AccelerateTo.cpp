#include "AccelerateTo.h"

//constructor
AccelerateTo::AccelerateTo(
	reb::Vector2 direction,
	std::pair<float, float> initialVelocityRange,
	std::pair<float, float> accelerationRange)
	:
	m_direction{ direction.setLength(1) },
	m_velocityRange{ initialVelocityRange },
	m_accelerationRange{ accelerationRange },
	m_currentVelocity{ reb::genRandf(initialVelocityRange.first, initialVelocityRange.second) },
	m_acceleration{ reb::genRandf(accelerationRange.first, accelerationRange.second) }
{};


//despawns the owner when it goes out of bounds
void AccelerateTo::update(float elapsed) {
	if (hasOwner()) {
		m_currentVelocity += m_acceleration * elapsed;
		getOwner()->m_transform.move(m_direction * m_currentVelocity * elapsed);
	}
};

//returns a clone of the component
AccelerateTo* AccelerateTo::clone() {
	return new AccelerateTo(m_direction, m_velocityRange, m_accelerationRange);
};

//returns a local aabb of the component (returns an empty aabb by default)
reb::Rect<float> AccelerateTo::getLocalAABB(float maxLifetime) {
	reb::Vector2 furthestPoint = m_direction * maxLifetime * maxLifetime * std::max(m_accelerationRange.first, m_accelerationRange.second) + m_direction * std::max(m_velocityRange.first, m_velocityRange.second);
	return reb::Rect<float>(std::min(furthestPoint.x, 0.f), std::min(furthestPoint.y, 0.f), std::abs(furthestPoint.x), std::abs(furthestPoint.y));
}