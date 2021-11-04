#include "ParticleEmitter.h"
#include "Container.h"

#include "objParticle.h"
#include "PixelParticle.h"

namespace reb {

	//constructor
	ParticleEmitter::ParticleEmitter(
		Particle* prototype, 
		int maxParticles,
		float maxPerUpdate,
		std::pair<float, float> lifetimeRange, 
		std::pair<Vector2, Vector2> posrange, 
		std::pair<float, float> scaleRange, 
		std::pair<float, float> rotationRange,
		ParticleRenderMode particleRenderMode,
		bool alwaysUpdate)
		: 
		m_prototype{prototype},
		m_posRange{ posrange },
		m_scaleRange{ scaleRange },
		m_rotationRange{ rotationRange },
		m_lifetimeRange{ lifetimeRange },
		MAXPARTICLES{ maxParticles },
		MAXPERUPDATE{maxPerUpdate },
		PARTICLERENDERMODE{ particleRenderMode },
		ALWAYSUPDATE{ alwaysUpdate },
		m_localAABB{
			std::min(posrange.first.x, posrange.second.x),
			std::min(posrange.first.y, posrange.second.y),
			std::max(posrange.first.x, posrange.second.x) - std::min(posrange.first.x, posrange.second.x),
			std::max(posrange.first.y, posrange.second.y) - std::min(posrange.first.y, posrange.second.y)
		}
	{ m_prototype->m_isPrototype = true; };

	void ParticleEmitter::onOwnerSet() {
		m_prototype->setOwner(this);
		m_prototype->updateEmitterAABB(getMaxLifetime());
	}

	//uodates the local aabb with the given aabb
;	void ParticleEmitter::updateLocalAABB(Rect<float> addittion) {
		if (ALWAYSUPDATE) { return; }
		Vector2 extremePositions[4] = {
			Vector2(std::min(m_posRange.first.x, m_posRange.second.x), std::min(m_posRange.first.y, m_posRange.second.y)),
			Vector2(std::max(m_posRange.first.x, m_posRange.second.x), std::min(m_posRange.first.y, m_posRange.second.y)),
			Vector2(std::max(m_posRange.first.x, m_posRange.second.x), std::max(m_posRange.first.y, m_posRange.second.y)),
			Vector2(std::min(m_posRange.first.x, m_posRange.second.x), std::max(m_posRange.first.y, m_posRange.second.y))
		};
		for (auto& basePos : extremePositions) {
			Rect<float> currentAdded = addittion;
			float left = m_localAABB.left;
			float right = m_localAABB.getRight();
			float top = m_localAABB.top;
			float bottom = m_localAABB.getBottom();
			left = std::min(left, currentAdded.left);
			right = std::max(right, currentAdded.getRight());
			top = std::min(top, currentAdded.top);
			bottom = std::max(bottom, currentAdded.getBottom());
			m_localAABB = Rect<float>(left, top, right - left, bottom - top);
		}
	}

	//spawns a particle
	void ParticleEmitter::spawnParticle() {
		//verifies there is an owner
		if (hasOwner()) {
			auto owner = getOwner();
			//creates the new particle
			auto clone = m_prototype->clone(
				genRandf(m_lifetimeRange.first, m_lifetimeRange.second),
				owner->getTransform()->getWorldSpace(
					Vector2{
						genRandf(m_posRange.first.x, m_posRange.second.x),
						genRandf(m_posRange.first.y, m_posRange.second.y)
					}
				),
				genRandf(m_scaleRange.first, m_scaleRange.second),
				genRandf(m_rotationRange.first, m_rotationRange.second)
			);
			addParticle(clone);
		}
	}

	//actually adds the particle to the emitter
	void ParticleEmitter::addParticle(Particle* particle) {
		//sets the owner to this
		particle->setOwner(this);
		//adds the particle to the vector
		m_particles.push_back(particle);
	}

	//returns true if a particle should be spawned
	bool ParticleEmitter::shouldSpawn() {
		return true;
	}

	//updates all the particles and spawns new ones
	void ParticleEmitter::update(float elapsed) {
		if (ALWAYSUPDATE || m_wasOnScreen) {
			preUpdate(elapsed);
			//updates all the particles
			for (auto& part : m_particles) {
				part->update(elapsed);
			}
			//despawns any particles that should be despawned
			finalDespawn();
			//spawns particles as long as shouldSpawn returns true and the amount of particles does not exceed the maximum
			int spawned = 0;
			while (m_enabled && m_particles.size() < MAXPARTICLES && spawned <= MAXPERUPDATE && shouldSpawn()) {
				spawned++;
				spawnParticle();
			}
			postUpdate(elapsed);
		}
	};

	//actually removes all despawned particles
	void ParticleEmitter::finalDespawn() {
		for (auto& part : m_toRemove) {
			auto iter = std::find(m_particles.begin(), m_particles.end(), part);
			bool isObjPart = typeid(*part) == typeid(ObjParticle);
			delete part;
			if (iter != m_particles.end()) {
				swapPopIter(m_particles, iter);
			}
		}
		m_toRemove.clear();
	}

	//enables the emitter
	void ParticleEmitter::enable() {
		m_enabled = true;
	}

	//disables the emitter
	void ParticleEmitter::disable() {
		m_enabled = false;
	}

	//toggles the emitter
	void ParticleEmitter::toggle() {
		m_enabled = !m_enabled;
	}

	//despawns the given particle
	void ParticleEmitter::despawn(Particle* particle) {
		if (!withinVector(m_toRemove, particle)) {
			m_toRemove.push_back(particle);
			onDespawn(particle);
		}
	}

	//draws all the particles to the screen
	void ParticleEmitter::draw(Renderer& renderer, const sf::FloatRect& viewBox) {
		Rect<float> currentAABB = m_localAABB + getOwner()->getTransform()->getPos();
		sf::FloatRect aabbRect{ sf::Vector2f(currentAABB.left, currentAABB.top), sf::Vector2f(currentAABB.width, currentAABB.height) };
		if (viewBox.intersects(aabbRect)) {
			m_wasOnScreen = true;
			DrawInfo drawInfo{};
			for (auto& part : m_particles) {
				part->draw(drawInfo, viewBox);
			}
			if (!drawInfo.m_verts.empty()) {
				drawInfo.m_shaded = false;
				drawInfo.m_transparent = true;
				drawInfo.setAutoBoundingBox();
				renderer.draw(drawInfo);
			}
		}
		else {
			m_wasOnScreen = false;
		}
		/*sf::RectangleShape aabb{ sf::Vector2f(m_localAABB.width,m_localAABB.height) };
		Vector2 pos{ m_localAABB.left, m_localAABB.top };
		aabb.setPosition((getOwner()->getTransform()->getPos() + pos).getSfml());
		aabb.setFillColor(sf::Color(255, 255, 255, 100));
		window.draw(aabb);*/
	};

	//returns the maximum lifetime any particle from this emitter can have
	float ParticleEmitter::getMaxLifetime(){
		return std::max(m_lifetimeRange.first, m_lifetimeRange.second);
	}

	//returns a copy of the emitter
	ParticleEmitter* ParticleEmitter::clone()const {
		Particle* clonedProto = m_prototype->clone(0, Vector2{}, 0, 0);
		clonedProto->m_isPrototype = true;
		return new ParticleEmitter(
			clonedProto,
			MAXPARTICLES, 
			MAXPERUPDATE, 
			m_lifetimeRange, 
			m_posRange, 
			m_scaleRange, 
			m_rotationRange, 
			PARTICLERENDERMODE
		);
	}

	//range getters
	const std::pair<Vector2, Vector2>& ParticleEmitter::getPosRange() { return m_posRange; }
	const std::pair<float, float>& ParticleEmitter::getScaleRange() { return m_scaleRange; }
	const std::pair<float, float>& ParticleEmitter::getRotationRange() { return m_rotationRange; }
	const std::pair<float, float>& ParticleEmitter::getLifeTimeRange() { return m_lifetimeRange; }

	//destroys the particle emitter
	void ParticleEmitter::destroy() {
		if (hasOwner()) {
			getOwner()->clearEmitter();
		}
	}

	//destructor
	ParticleEmitter::~ParticleEmitter() {
		//frees memory for all the particles
		for (auto& part : m_particles) {
			delete part;
		}
		//frees memory for the prototype particle
		delete m_prototype;
	};

}