#pragma once
#include <reb/GameObj.h>
#include <functional>

class LambdaTrigger : public reb::GameObj {
public:
	//typedef for better syntax
	typedef std::function<void(LambdaTrigger* object, Container* other)> TriggerFunction;

private:
	//the enter and leave functions
	TriggerFunction m_triggerEnter;
	TriggerFunction m_triggerLeave;

	//wether the functions should be used or not
	bool m_useEnter = true;
	bool m_useLeave = true;
protected:
	//calls the trigger enter function, if it ever throws a bad_function_call it wont be used again
	void triggerEnter(reb::Container* other)override;

	//calls the trigger leave function, if it ever throws a bad_function_call it wont be used again
	void triggerLeave(reb::Container* other)override;

public:
	//constructor
	LambdaTrigger(
		TriggerFunction triggerEnter,
		TriggerFunction triggerLeave,
		reb::Transform* transform,
		reb::Container::BoundType boundType = reb::Container::BoundType::BOUNDS_RIGIDBODY
	);

};