#pragma once
#include <reb/ParticleComp.h>

class AccelerateTo : public reb::ParticleComp {
private:
	//movement information
	reb::Vector2 m_direction;
	float m_currentVelocity;
	float m_acceleration;
	
	//copying information
	std::pair<float, float> m_velocityRange;
	std::pair<float, float> m_accelerationRange;

protected:
	//despawns the owner when it goes out of bounds
	void update(float elapsed)override;

	//returns a clone of the component
	AccelerateTo* clone()override;

	//returns a local aabb of the component (returns an empty aabb by default)
	reb::Rect<float> getLocalAABB(float maxLifetime)override;

public:
	//constructor
	AccelerateTo(
		reb::Vector2 direction,
		std::pair<float, float> initialVelocityRange,
		std::pair<float, float> accelerationRange
	);


};