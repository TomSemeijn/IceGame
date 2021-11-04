#include "LambdaTrigger.h"

//constructor
LambdaTrigger::LambdaTrigger(
	TriggerFunction triggerEnter,
	TriggerFunction triggerLeave,
	reb::Transform* transform,
	reb::Container::BoundType boundType)

	: m_triggerEnter{ triggerEnter },
	m_triggerLeave{ triggerLeave },
	reb::GameObj(transform, boundType)
{};

//calls the trigger enter function, if it ever throws a bad_function_call it wont be used again
void LambdaTrigger::triggerEnter(reb::Container* other) {
	if (m_useEnter) {
		try {
			m_triggerEnter(this, other);
		}
		catch (std::bad_function_call) {
			m_useEnter = false;
		}
	}
};

//calls the trigger leave function, if it ever throws a bad_function_call it wont be used again
void LambdaTrigger::triggerLeave(reb::Container* other) {
	if (m_useLeave) {
		try {
			m_triggerLeave(this, other);
		}
		catch (std::bad_function_call) {
			m_useLeave = false;
		}
	}
};