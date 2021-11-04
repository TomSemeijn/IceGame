#pragma once
#include <reb/GameObj.h>
#include <functional>

class LambdaCollide : public reb::GameObj {
public:
	//typedef for better syntax
	typedef std::function<bool(reb::Container*)> CanCollideFunction;

private:
	//the funciton
	CanCollideFunction m_canCollide;

	//true if the function should actually be used
	bool m_useFunc = true;

protected:
	//tries to use the lambda function to determine if this object can collide with the other or not,
	//returns true if the function shouldn't be used due to it being a bad function call
	bool canCollide(reb::Container* other)override;

public:
	//constructor
	LambdaCollide(
		CanCollideFunction canCollide,
		reb::Transform* transform,
		reb::Container::BoundType boundType = reb::Container::BoundType::BOUNDS_RIGIDBODY
	);
};