#ifndef REBOOTOBJPARTICLE_H
#define REBOOTOBJPARTICLE_H

#pragma once
#include "Particle.h"
#include "GameObj.h"
#include "ParticleComp.h"

namespace reb {

	class ObjParticle;

	//basic empty particle owned object
	class BasicParticleObj : public GameObj {
		friend ObjParticle;
	private:
		//the owning object particle
		ObjParticle* m_objParticleOwner = nullptr;
	public:
		//constructor
		BasicParticleObj(Transform* transform = new Transform(true, true, true), BoundType boundType = BoundType::BOUNDS_RIGIDBODY);

		//copy constructor
		BasicParticleObj(const BasicParticleObj& other);

		//returns a clone
		virtual BasicParticleObj* clone();

		//destructor
		virtual ~BasicParticleObj();
	};

	//a particle that owns an object in the scene
	class ObjParticle : public Particle {
		friend BasicParticleObj;
	protected:
		//the prototype and actual object of the particle
		BasicParticleObj* m_prototype;
		BasicParticleObj* m_object = nullptr;

		//creates the object if necessary and possible
		void onOwnerSet()override;

		//syncs up the game object with the particle on update
		void onUpdate(float elapsed)override;
	public:
		//true if the object position should sync with the particle position
		const bool SYNCPOSITION;

		//default constructor
		ObjParticle(BasicParticleObj* prototype, bool syncPos = true, Color color = Color{ 255, 255, 255 });
		//the constructor
		ObjParticle(float lifeTime, Vector2 pos, float scale, float rotation, BasicParticleObj* prototype, bool syncPos = true, Color color = Color(255, 255, 255));

		//draws the particle to the screen
		void draw(DrawInfo& drawInfo, const sf::FloatRect& viewBox)override;

		//returns a clone of the particle with the given transform info
		Particle* clone(float lifeTime, Vector2 pos, float scale, float rotation)override;

		//returns a const pointer to the object the particle manages
		BasicParticleObj* getObject();

		//destructor
		virtual ~ObjParticle();
	};

}

#endif