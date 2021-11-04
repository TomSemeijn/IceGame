#ifndef REBOOTMATERIAL_H
#define REBOOTMATERIAL_H

#pragma once

namespace reb {

	//a material that a rigidbody is made of, used to calculate how to react to collisions
	struct Material {

		//material info
		float density;
		float restitution;
		float staticFriction;
		float dynamicFriction;

		//constructor
		Material(float Density, float Restitution, float StaticFriction, float DynamicFriction)
			:	density{Density},
				restitution{Restitution},
				staticFriction{StaticFriction},
				dynamicFriction{DynamicFriction}
		{};

	};

}

#endif