#include "Particle.h"
#include "ParticleComp.h"
#include "ParticleEmitter.h"

#include <chrono>

namespace reb {

	//updates the aabb of the owning emitter using every component of the particle
	void Particle::updateEmitterAABB(float maxLifetime) {
		for (auto& comp : m_components) {
			updateEmitterAABB(maxLifetime, comp);
		}
	}

	//updates the aabb of the owning particle using a specific component
	void Particle::updateEmitterAABB(float maxLifetime, ParticleComp* comp) {
		if (hasOwner()) {
			getOwner()->updateLocalAABB(comp->getLocalAABB(maxLifetime));
		}
	}

	//adds clones of all components from the given particle to this particle
	void Particle::cloneComponentsFrom(Particle* other) {
		//loops through all components
		for (auto& comp : other->m_components) {
			//verifies the current component isnt going to be removed
			if (std::find(other->m_toRemove_components.begin(), other->m_toRemove_components.end(), comp) == other->m_toRemove_components.end()) {
				//adds the current component
				m_components.push_back(comp->clone());
				//sets owner of component
				m_components.back()->setOwner(this);
			}
		}
	}

	//returns the amount of time the particle has existed
	float Particle::getTime() {
		return m_time;
	}

	//updates the particle using its components
	void Particle::update(float elapsed) {
		//increments time
		m_time += elapsed;
		//updates components
		for (auto& comp : m_components) {
			comp->update(elapsed);
		}
		//removes any components that should be removed
		if (!m_toRemove_components.empty()) {
			finalRemoveComp();
		}
		//despawns the particle if necessary
		if (LIFETIME > 0 && LIFETIME - m_time < EPSILON) {
			destroy();
		}
		//calls onUpdate
		onUpdate(elapsed);
	}

	//adds a components to the particle
	void Particle::addComp(ParticleComp* newComp) {
		//verifies the component exists and is not yet in the vector
		if (newComp != nullptr && !hasComp(newComp)) {
			//adds the component
			m_components.push_back(newComp);
			//sets the owner of the component to this
			newComp->setOwner(this);
			//updates emitter aabb if its a prototype
			if (m_isPrototype && hasOwner()) {
				updateEmitterAABB(getOwner()->getMaxLifetime(), newComp);
			}
		}
	}

	//returns true if the particle has the given component
	bool Particle::hasComp(ParticleComp* comp) {
		return std::find(m_components.begin(), m_components.end(), comp) != m_components.end();
	}

	//removes a component from the particle
	void Particle::removeComp(ParticleComp* comp) {
		//verifies the particle has this component
		if (hasComp(comp)) {
			//adds the component to the toRemove vector
			m_toRemove_components.push_back(comp);
		}
	}

	//executes a function for every component
	void Particle::foreachComp(std::function<void(ParticleComp&)> func) {
		for (auto& comp : m_components) {
			func(*comp);
		}
	}

	//removes all components that should be removed at the end of the update
	void Particle::finalRemoveComp() {
		if (!m_toRemove_components.empty()) {
			//loops through all components to remove
			for (auto& comp : m_toRemove_components) {
				//attempts to find the component
				auto iter = std::find(m_components.begin(), m_components.end(), comp);
				//verifies the component was found
				if (iter != m_components.end()) {
					//frees the memory
					delete comp;
					//removes the component from the vector
					swapPopIter(m_components, iter);
				}
			}
			//clears the vector
			m_toRemove_components.clear();
			//updates emitter aabb if its a prototype
			if (m_isPrototype) {
				updateEmitterAABB(getOwner()->getMaxLifetime());
			}
		}
	}

	//removes the particle
	void Particle::destroy() {
		if (hasOwner()) {
			getOwner()->despawn(this);
		}
	}

	//destructor
	Particle::~Particle() {
		//sets all components to be removed
		for (auto& comp : m_components) {
			delete comp;
		}
	}

}