#ifndef REBOOTPARTICLECOMP_H
#define REBOOTPARTICLECOMP_H

#pragma once
#include "Particle.h"
#include "OwnedObject.h"

namespace reb {

	//the abstract base of a component that can update a particle
	class ParticleComp : public OwnedObject<Particle> {
	public:

		//updates the component to allow behavior to be added to the owning particle
		virtual void update(float elapsed) = 0;

		//returns a clone of the component
		virtual ParticleComp* clone() = 0;

		//returns a local aabb of the component (returns an empty aabb by default)
		virtual Rect<float> getLocalAABB(float maxLifetime) { return Rect<float>(); };

		//removes the component from the owning particle
		void destroy()override {
			if (hasOwner()) {
				getOwner()->removeComp(this);
			}
		};

		//virtual destructor
		virtual ~ParticleComp() {};
	};

}

#endif