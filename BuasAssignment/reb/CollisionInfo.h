#ifndef REBOOTCOLLISIONINFO_H
#define REBOOTCOLLISIONINFO_H

#pragma once
#include <array>
#include "Rigidbody.h"

namespace reb {

	//contains information about a collision
	struct CollisionInfo {

		//the allowed amount of penetration
		const float SLOP = 0.01;
		//the measured percentage of penetration
		const float SLOPCHECK = 0.4;
		
		//the rigidbodies involved in the collision
		Rigidbody* a;
		Rigidbody* b;

		//the amount of penetration of this collision
		float penetration = 0;
		//the normal of the collision
		Vector2 normal{};

		//the points of contact after the collision
		std::vector<Vector2> contactPoints{};

		//the values processed from the materials involved in the collision
		float restitution = 0;
		float staticFriction = 0;
		float dynamicFriction = 0;

		//constructor
		CollisionInfo(Rigidbody* A, Rigidbody* B) : a{ A }, b{ B }{ };

		//sets all the values related to the materials of the rigidbodies
		void setMaterialInfo(float elapsed);
		//applies the impulse necessary to separate the rigidbodies
		void addImpulse(float elapsed);

		//applies positional correction
		void positionalCorrection();

	};
};

#endif