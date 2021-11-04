#include "LambdaCollide.h"

//constructor
LambdaCollide::LambdaCollide(
	CanCollideFunction canCollide,
	reb::Transform* transform,
	reb::Container::BoundType boundType)

	:
	m_canCollide{ canCollide },
	reb::GameObj(transform, boundType)
{};

//tries to use the lambda function to determine if this object can collide with the other or not,
//returns true if the function shouldn't be used due to it being a bad function call
bool LambdaCollide::canCollide(reb::Container* other) {
	if (m_useFunc) {
		try {
			return m_canCollide(other);
		}
		catch (std::bad_function_call) {
			m_useFunc = false;
		}
	}
	return true;
}