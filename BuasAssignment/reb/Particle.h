#ifndef REBOOTPARTICLE_H
#define REBOOTPARTICLE_H

#pragma once
#include <functional>
#include <SFML/Graphics.hpp>
#include "Transform.h"
#include "Color.h"
#include "OwnedObject.h"
#include "DrawBatch.h"

namespace reb {

	class ParticleComp;
	class ParticleEmitter;

	//the abstract base class of all the particles
	class Particle : public OwnedObject<ParticleEmitter>{
		friend ParticleEmitter;
	private:
		//the components that influence the behavior of the particles
		std::vector<ParticleComp*> m_components;

		//the amount of time the particle has existed
		float m_time;

		//updates the aabb of the owning emitter using every component of the particle
		void updateEmitterAABB(float maxLifetime);
		//updates the aabb of the owning particle using a specific component
		void updateEmitterAABB(float maxLifetime, ParticleComp* comp);

		//keeps a vector of all components to be removed
		std::vector<ParticleComp*> m_toRemove_components;
		//removes all components that should be removed at the end of the update
		void finalRemoveComp();
	protected:
		//the color the particle started with
		const Color ORIGINALCOLOR;

		//true if the particle was constructed with the default constructor, meaning its a prototype
		bool m_isPrototype;

		//adds clones of all components from the given particle to this particle
		void cloneComponentsFrom(Particle* other);

		//allows child classes to add to the update function
		virtual void onUpdate(float elapsed) {};
	public:
		//the transform of the particle
		Transform m_transform;
		//the color of the particle
		Color m_color;

		//the amount of time the particle will exist in seconds
		const float LIFETIME;

		//default constructor
		Particle(Color color = Color{ 255, 255, 255 }) : LIFETIME{ 0 }, m_color{ color }, m_isPrototype{ true }, ORIGINALCOLOR{ color } {};

		//the constructor
		Particle(float lifeTime, Vector2 pos, float scale, float rotation, Color color = Color{ 255, 255, 255 }) : m_transform{ true, true, true, pos, scale, rotation }, LIFETIME{ lifeTime }, m_color{ color }, m_isPrototype{ false }{};

		//updates the particle using its components
		void update(float elapsed);

		//adds a components to the particle
		void addComp(ParticleComp* newComp);
		//returns true if the particle has the given component
		bool hasComp(ParticleComp* comp);
		//removes a component from the particle
		void removeComp(ParticleComp* comp);

		//executes a function for every component
		void foreachComp(std::function<void(ParticleComp&)> func);

		//returns the amount of time the particle has existed
		float getTime();

		//draws the particle to the screen
		virtual void draw(DrawInfo& drawInfo, const sf::FloatRect& viewBox) = 0;

		//returns a clone of the particle with the given transform info
		virtual Particle* clone(float lifeTime, Vector2 pos, float scale, float rotation) = 0;

		//removes the particle
		void destroy()override;
		
		//destructor
		virtual ~Particle();
	};

}

#endif