#pragma once
#include <reb/Particles/ObjSync.h>

//syncs the position of the particle to match that of its object
class ObjScaleSync : public reb::Particles::ObjSync {
public:
	//syncs on update
	void update(float elapsed)override;

	//clones the component
	ObjScaleSync* clone()override;
};