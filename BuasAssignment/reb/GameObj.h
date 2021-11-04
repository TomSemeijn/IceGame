#ifndef REBOOTGAMEOBJ_H
#define REBOOTGAMEOBJ_H

#pragma once
#include "Container.h"

namespace reb {

	//the main type of game object, owned by scene
	class GameObj : public Container, public OwnedObject<Scene> {
		using OwnedObject<Scene>::destroy;
	protected:
		//copy constructor
		GameObj(const GameObj &obj);
	public:
		//constructors
		GameObj(BoundType boundType = BoundType::BOUNDS_RIGIDBODY);
		GameObj(Transform* transform, BoundType boundType = BoundType::BOUNDS_RIGIDBODY);

		//called before the main update method
		virtual void earlyUpdate(float elapsed) {};

		//called after the main update method
		virtual void lateUpdate(float elapsed) {};

		//removes this object from the scene
		virtual void destroy()override;

		//returns a clone of the object
		virtual GameObj* clone();

		//returns the scene
		virtual Scene* getScene()override;
	};

}

#endif