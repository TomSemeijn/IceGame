#pragma once
#include <reb/Particles/ObjSync.h>

//syncs the position of the particle to match that of its object
class OpacityScaleSync : public reb::ParticleComp {
public:
	//syncs on update
	void update(float elapsed)override;

	//clones the component
	OpacityScaleSync* clone()override;
};