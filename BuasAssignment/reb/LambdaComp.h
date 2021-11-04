#ifndef REBOOTLAMBDACOMP_H
#define REBOOTLAMBDACOMP_H

#pragma once
#include "Component.h"
#include <functional>

namespace reb {

	//a simple extension of component where a lambda can be given to define the update function
	class LambdaComp : public Component {
	protected:
		//the lambda
		std::function<void(float, Container*const)> m_callback;
	public:
		//constructor
		LambdaComp(std::function<void(float, Container*const)> callback) : m_callback{ callback } {};

		//calls the lambda in update
		virtual void update(float elapsed)override { m_callback(elapsed, getOwner()); };

		//returns a copy of the component
		virtual MainComponent* clone()const override { return new LambdaComp{ *this }; };
	};

}

#endif