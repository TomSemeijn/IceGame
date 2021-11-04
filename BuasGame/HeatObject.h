#pragma once
#include <reb/GameObj.h>
#include "HeatHolder.h"

class HeatObj : public reb::GameObj, public HeatHolder {
public:
	//constructor
	HeatObj(Level* level, HeatEmitter* heat, BoundType boundType = BoundType::BOUNDS_RIGIDBODY);
	HeatObj(Level* level, HeatEmitter* heat, reb::Transform* transform, BoundType boundType = BoundType::BOUNDS_RIGIDBODY);

};