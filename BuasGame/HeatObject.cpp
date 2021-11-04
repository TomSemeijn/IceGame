#include "HeatObject.h"

//constructor
HeatObj::HeatObj(Level* level, HeatEmitter* heat, BoundType boundType)
	: reb::GameObj(boundType),
	HeatHolder(level, heat)
{};
HeatObj::HeatObj(Level* level, HeatEmitter* heat, reb::Transform* transform, BoundType boundType)
	: reb::GameObj(transform, boundType),
	HeatHolder(level, heat)
{};