#pragma once
#include <reb/ParticleComp.h>

class MoveFromPoint : public reb::ParticleComp {
private:
	reb::Vector2 m_point;
	std::pair<float, float> m_velocityRange;

	reb::Vector2 m_moveVec;
	bool m_moveVecSet = false;

protected:
	//despawns the owner when it goes out of bounds
	void update(float elapsed)override;

	//returns a clone of the component
	MoveFromPoint* clone()override;

	//returns a local aabb of the component (returns an empty aabb by default)
	reb::Rect<float> getLocalAABB(float maxLifetime)override;

public:
	//constructor
	MoveFromPoint(reb::Vector2 point, std::pair<float, float> velocityRange);


};