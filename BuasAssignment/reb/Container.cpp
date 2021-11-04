#include "Container.h"
#include "Transform.h"
#include "Scene.h"
#include "LightSource.h"
#include "ParticleEmitter.h"
#include "Drawable.h"

namespace reb {

	//sets the default layer to 0
	LayerIndex Container::s_defaultLayer = (LayerIndex)0;

	//default constructor
	Container::Container(BoundType boundType) : m_transform{ new Transform{} }, BOUNDTYPE{ boundType } {
		m_transform->setOwner(this);
	}

	//constructor taking transform
	Container::Container(Transform* transform, BoundType boundType) : m_transform{ transform }, BOUNDTYPE{ boundType } {
		m_transform->setOwner(this);
	}

	//copy constructor
	Container::Container(const Container& original) 
		:	BOUNDTYPE{ original.BOUNDTYPE }, 
			m_layer{ original.m_layer },
			m_transform{ (Transform*)original.getTransform()->clone() }
	{
		m_transform->setOwner(this);
	}

	//copies main components from the referenced container
	void Container::copyMainComponentsFrom(const Container& other) {
		auto spr = other.getSprite();
		if (spr != nullptr) {
			setSprite((Drawable*)spr->clone());
		}
		auto body = other.getBody();
		if (body != nullptr) {
			setBody((Rigidbody*)body->clone());
		}
		auto light = other.getLight();
		if (light != nullptr) {
			setLight((LightSource*)light->clone());
		}
		auto emit = other.getEmitter();
		if (emit != nullptr) {
			setEmitter((ParticleEmitter*)emit->clone());
		}
		auto comps = other.getConstComponents();
		for (auto& comp : comps) {
			addComponent((Component*)comp->clone());
		}
	}

	//allows child classes to change position without permission from transform
	void Container::forcePos(Vector2 newPos) {
		m_transform->m_pos = newPos;
	}

	//allows child classes to change size without permission from transform
	void Container::forceSize(float newSize) {
		m_transform->m_scale = newSize;
	}

	//allows child classes to change rotation without permission from transform
	void Container::forceRotation(float newRotation) {
		m_transform->m_rotation = newRotation;
	}

	//updates the container
	void Container::updateContainer(float elapsed) {
		//updates every component
		for (auto& comp : m_components) {
			comp->update(elapsed);
		}
		//updates the particle emitter if there is one
		if (m_particleEmitter != nullptr) {
			m_particleEmitter->update(elapsed);
		}
		//calls update for child classes
		update(elapsed);
		//calls finalRemoveComponents in case any components need to be removed
		finalRemoveComponents();
		finalAddComponents();
		finalizeMainComponents();
	}

	//removes all components that should be removed
	void Container::finalRemoveComponents() {
		//verifies there are components to be removed
		if (!m_toRemove_components.empty()) {
			//loops through all components to be removed
			for (auto& remove : m_toRemove_components) {
				//verifies the component isnt nullptr
				if (remove != nullptr) {
					//frees memory
					delete remove;
					//removes from the main set
					auto iter = std::find(m_components.begin(), m_components.end(), remove);
					if (iter != m_components.end()) {
						m_components.erase(iter);
					}
				}
			}
			//clears the components to remove set
			m_toRemove_components.clear();
		}
	}

	//adds all the components that should be added
	void Container::finalAddComponents() {
		if (!m_toAdd_components.empty()) {
			for (auto& comp : m_toAdd_components) {
				if (!withinVector(m_components, comp)) {
					m_components.push_back(comp);
				}
			}
			m_toAdd_components.clear();
		}
	}

	//returns the transform
	Transform* Container::getTransform()const {
		return m_transform;
	}

	//returns the sprite
	Drawable* Container::getSprite()const {
		return m_sprite;
	}

	//returns the lightsource
	LightSource* Container::getLight()const {
		return m_lightsource;
	}

	//returns the rigidbody
	Rigidbody* Container::getBody()const {
		return m_rigidbody;
	}

	//returns the particle emitter
	ParticleEmitter* Container::getEmitter()const {
		return m_particleEmitter;
	}

	//returns a reference to the component set
	std::vector<Component*>& Container::getComponents() {
		return m_components;
	}

	const std::vector<Component*>& Container::getConstComponents()const {
		return m_components;
	}

	//returns the layer
	LayerIndex Container::getLayer()const {
		return m_layer;
	}

	//sets the sprite
	void Container::setSprite(Drawable* newSprite) {
		//clears the current sprite
		clearSprite();
		if (newSprite != nullptr) {
			//sets the new sprite
			if (m_sprite == nullptr) {
				m_sprite = newSprite;
			}
			else {
				m_replacement_sprite = newSprite;
			}
			newSprite->setOwner(this);
		}
	}

	//sets the lightsource
	void Container::setLight(LightSource* newLight) {
		//clears the current light
		clearLight();
		if (newLight != nullptr) {
			//sets the new light
			if (m_lightsource == nullptr) {
				m_lightsource = newLight;
			}
			else {
				m_replacement_lightsource = newLight;
			}
			newLight->setOwner(this);
		}
	}

	//sets the rigidbody
	void Container::setBody(Rigidbody* newBody) {
		//clears the current body
		clearBody();
		if (newBody != nullptr) {
			//sets the new body
			if (m_rigidbody == nullptr) {
				m_rigidbody = newBody;
			}
			else {
				m_replacement_rigidbody = newBody;
			}
			newBody->setOwner(this);
		}
	}

	//sets the particle emitter
	void Container::setEmitter(ParticleEmitter* newEmitter) {
		//clears the current emitter
		clearEmitter();
		if (newEmitter != nullptr) {
			//sets the new emitter
			if (m_particleEmitter == nullptr) {
				m_particleEmitter = newEmitter;
			}
			else {
				m_replacement_particleEmitter = newEmitter;
			}
			newEmitter->setOwner(this);
		}
	}

	//adds a component
	void Container::addComponent(Component* newComp) {
		if (!withinVector(m_toAdd_components, newComp)) {
			m_components.push_back(newComp);
			newComp->setOwner(this);
		}
	}

	//sets the layer
	void Container::setLayer(LayerIndex newLayer) {
		m_layer = newLayer;
	}

	//clears the sprite
	void Container::clearSprite() {
		//verifies there is a sprite
		if (m_sprite != nullptr) {
			m_clearSprite = true;
		}
	}

	//clears the lightsource
	void Container::clearLight() {
		//verifies there is a light
		if (m_lightsource != nullptr) {
			m_clearLight = true;
		}
	}

	//clears the rigidbody
	void Container::clearBody() {
		//verifies there is a body
		if (m_rigidbody != nullptr) {
			m_clearBody = true;
		}
	}

	//clears the particle emitter
	void Container::clearEmitter() {
		//verifies there is an emitter
		if (m_particleEmitter != nullptr) {
			m_clearEmitter = true;
		}
	}

	//clears the components
	void Container::clearComponents() {
		//loops through all components
		for (auto& comp : m_components) {
			//removes component
			removeComponent(comp);
		}
	}

	//removes a specific components
	void Container::removeComponent(Component* toRemove) {
		//adds the component to the toRemove set
		if (!withinVector(m_toRemove_components, toRemove)) {
			m_toRemove_components.push_back(toRemove);
		}
	}

	//finalizes removing and setting main components
	void Container::finalizeMainComponents() {

		//clears what should be cleared
		if (m_clearBody) {
			m_rigidbody->m_owner = nullptr;
			delete m_rigidbody;
			m_rigidbody = nullptr;
			m_clearBody = false;
		}
		if (m_clearLight) {
			m_lightsource->m_owner = nullptr;
			delete m_lightsource;
			m_lightsource = nullptr;
			m_clearLight = false;
		}
		if (m_clearSprite) {
			m_sprite->m_owner = nullptr;
			delete m_sprite;
			m_sprite = nullptr;
			m_clearSprite = false;
		}
		if (m_clearEmitter) {
			m_particleEmitter->m_owner = nullptr;
			delete m_particleEmitter;
			m_particleEmitter = nullptr;
			m_clearEmitter = false;
		}

		//replaces what should be replaced
		if (m_replacement_rigidbody != nullptr) {
			m_rigidbody = m_replacement_rigidbody;
			m_replacement_rigidbody = nullptr;
		}
		if (m_replacement_lightsource != nullptr) {
			m_lightsource = m_replacement_lightsource;
			m_replacement_lightsource = nullptr;
		}
		if (m_replacement_sprite != nullptr) {
			m_sprite = m_replacement_sprite;
			m_replacement_sprite = nullptr;
		}
		if (m_replacement_particleEmitter != nullptr) {
			m_particleEmitter = m_replacement_particleEmitter;
			m_replacement_particleEmitter = nullptr;
		}
	}

	//destructor
	Container::~Container() {
		//clears any possible components, which also frees their memory
		clearBody();
		clearLight();
		clearSprite();
		clearEmitter();
		clearComponents();
		//calls finalRemoveComponents to also free memory of the components
		finalRemoveComponents();
		finalizeMainComponents();
		//frees memory of the transform
		delete m_transform;
	}

}