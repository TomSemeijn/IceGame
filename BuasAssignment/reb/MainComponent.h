#ifndef REBOOTMAINCOMPONENT_H
#define REBOOTMAINCOMPONENT_H

#pragma once
#include "OwnedObject.h"

namespace reb {

	class Container;

	//abstract base class that forces the main components of Container to define a clone function
	class MainComponent : public OwnedObject<Container> {
	public:
		//returns a copy of the component
		virtual MainComponent* clone()const = 0;
	};

}

#endif