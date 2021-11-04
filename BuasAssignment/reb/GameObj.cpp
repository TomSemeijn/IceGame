#include "GameObj.h"
#include "Rigidbody.h"
#include "Scene.h"

#include "Functions.h"

namespace reb {

	//constructors
	GameObj::GameObj(BoundType boundType) : Container(boundType) {};
	GameObj::GameObj(Transform* transform, BoundType boundType) : Container(transform, boundType) {};

	//copy constructor
	GameObj::GameObj(const GameObj& obj) : Container{ obj } { copyMainComponentsFrom(obj); }

	//returns a clone of the object
	GameObj* GameObj::clone() {
		GameObj* newObj = new GameObj(*this);
		newObj->copyMainComponentsFrom(*this);
		return newObj;
	}

	//removes this object from the scene (only works if it's actually within a scene to begin with)
	void GameObj::destroy() {
		//removes from the scene if there is one
		if (hasOwner()) {
			getOwner()->removeGameObj(this);
		}
	}

	//returns the scene
	Scene* GameObj::getScene() {
		return hasOwner() ? getOwner() : nullptr;
	}

}