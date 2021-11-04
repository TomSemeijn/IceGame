#ifndef REBOOTPARTICLEEMITTER_H
#define REBOOTPARTICLEEMITTER_H

#pragma once
#include "MainComponent.h"
#include <SFML/Graphics.hpp>
#include "Particle.h"
#include "Functions.h"
#include "Renderer.h"

namespace reb {

	class Scene;

	//spawns, despawns, updates and drawns particles
	//stops spawning particles when disabled, but doesnt stop updating while on-screen
	class ParticleEmitter : public MainComponent {

	private:
		//a boolean for if the emitter is currently enabled or not
		bool m_enabled = true;

		//the local aabb of the emitter
		Rect<float> m_localAABB;
		//true if the emitter was on screen last render
		bool m_wasOnScreen = true;

		//the ranges of basic transform values used for spawning particles
		std::pair<Vector2, Vector2> m_posRange;
		std::pair<float, float> m_scaleRange;
		std::pair<float, float> m_rotationRange;
		std::pair<float, float> m_lifetimeRange;

		//all particles that should be despawned at the end of the update
		std::vector<Particle*> m_toRemove;
		//actually removes all despawned particles
		void finalDespawn();
	protected:
		//vector of currently spawned particles
		std::vector<Particle*> m_particles;

		//particle prototype
		Particle* m_prototype;

		//called before an update
		virtual void preUpdate(float elapsed) {};

		//called after an update
		virtual void postUpdate(float elapsed) {};

		//spawns a particle
		virtual void spawnParticle();

		//actually adds the particle to the emitter
		void addParticle(Particle* particle);

		//returns true if a particle should be spawned
		virtual bool shouldSpawn();

		//called when a particle is despawned
		virtual void onDespawn(Particle* despawned) {};

		//sets the owner of the particle to this when the owner of the emitter is set
		void onOwnerSet()override;
	public:
		//enum for drawing the particles on top of or behind the sprite
		enum ParticleRenderMode {
			PARTICLE_RENDER_TOP,
			PARTICLE_RENDER_BOTTOM
		};
		const ParticleRenderMode PARTICLERENDERMODE;
		const int MAXPARTICLES;
		const float MAXPERUPDATE;
		const bool ALWAYSUPDATE;

		//constructor
		ParticleEmitter(
			Particle* prototype,
			int maxParticles,
			float maxPerUpdate,
			std::pair<float, float> lifetimeRange,
			std::pair<Vector2, Vector2> posrange,
			std::pair<float, float> scaleRange,
			std::pair<float, float> rotationRange,
			ParticleRenderMode particleRenderMode = ParticleRenderMode::PARTICLE_RENDER_TOP,
			bool alwaysUpdate = false
		);

		//uodates the local aabb with the given aabb
		void updateLocalAABB(Rect<float> addittion);

		//returns a copy of the emitter
		ParticleEmitter* clone()const override;

		//updates all the particles and spawns new ones
		void update(float elapsed);

		//draws all the particles to the screen
		virtual void draw(Renderer& renderer, const sf::FloatRect& viewBox);

		//despawns the given particle
		void despawn(Particle* particle);

		//returns the maximum lifetime any particle from this emitter can have
		float getMaxLifetime();

		//enable/disable functions
		void enable();
		void disable();
		void toggle();

		//range getters
		const std::pair<Vector2, Vector2>& getPosRange();
		const std::pair<float, float>& getScaleRange();
		const std::pair<float, float>& getRotationRange();
		const std::pair<float, float>& getLifeTimeRange();

		//destroys the particle emitter
		void destroy()override;

		//destructor
		virtual ~ParticleEmitter();
	};

}

#endif