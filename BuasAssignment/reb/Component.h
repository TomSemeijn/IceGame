#ifndef REBOOTCOMPONENT_H
#define REBOOTCOMPONENT_H

#pragma once
#include "MainComponent.h"

namespace reb {

	class Container;

	//the base component class
	//a component can be added to a game object to influence its behavior through the update function
	class Component : public MainComponent{
		friend Container;
	protected:
		//gets called every update
		virtual void update(float elapsed) = 0;

		//removes the component from its owning object
		void destroy()override;

		//virtual destructor
		virtual ~Component() {};
	};

}

#endif