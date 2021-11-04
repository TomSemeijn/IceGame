#include "objParticle.h"
#include "Particles/ObjSync.h"
#include "ParticleEmitter.h"
#include "Scene.h"
#include "Drawable.h"
#include "Tile.h"
#include "Tileset.h"
#include "Functions.h"

namespace reb {

	//======================== basic particle object ==================================================

	//constructor
	BasicParticleObj::BasicParticleObj(Transform* transform, BoundType boundType) : GameObj(transform, boundType) {};

	//copy constructor
	BasicParticleObj::BasicParticleObj(const BasicParticleObj& other) : GameObj{ other } {};

	//returns a clone
	BasicParticleObj* BasicParticleObj::clone() { return new BasicParticleObj(*this); };

	//destructor
	BasicParticleObj::~BasicParticleObj() {
		//removes itself from its object particle owner if there is one
		if (m_objParticleOwner != nullptr) {
			m_objParticleOwner->m_object = nullptr;

			//removes itself from any components that inherit from ObjSync
			m_objParticleOwner->foreachComp([](ParticleComp& comp) {
				if (Particles::ObjSync* cast = dynamic_cast<Particles::ObjSync*>(&comp)) {
					cast->m_objectOwner = nullptr;
				}
			});

		}
	}

	//======================== object particle ========================================================

	//creates the object if necessary and possible
	void ObjParticle::onOwnerSet() {
		if (!m_isPrototype) {
			ParticleEmitter* owner = getOwner();
			if (owner->hasOwner()) {
				Container* container = owner->getOwner();
				if (Scene* scene = container->getScene()) {
					//adds the object
					m_object = (BasicParticleObj*)scene->addGameObj(m_prototype->clone());
					m_object->m_objParticleOwner = this;
					//performs an initial sync
					update(0);
				}
				else {
					throw std::out_of_range("Couldn't add the object of an ObjParticle to the scene because the scene couldnt be found through the owning container");
				}
			}
			else {
				throw std::out_of_range("Couldn't add the object of an ObjParticle to the scene because there was no ParticleEmitter owner set");
			}
		}
	}

	//default constructor
	ObjParticle::ObjParticle(BasicParticleObj* prototype, bool syncPos, Color color) : Particle{ color }, m_prototype{ prototype }, SYNCPOSITION{ syncPos } { };
	//the constructor
	ObjParticle::ObjParticle(float lifeTime, Vector2 pos, float scale, float rotation, BasicParticleObj* prototype, bool syncPos, Color color) : Particle{ lifeTime, pos, scale, rotation, color }, m_prototype{ prototype }, SYNCPOSITION{ syncPos } {
		m_prototype->getTransform()->setPos(pos);
		m_prototype->getTransform()->setScale(scale);
		m_prototype->getTransform()->setRotation(rotation);
	};

	//syncs up the game object with the particle on update
	void ObjParticle::onUpdate(float elapsed) {
		//verifies there is an object
		if (SYNCPOSITION && m_object != nullptr) {
			//syncs the transform info
			auto trans = m_object->getTransform();
			trans->setPos(m_transform.getPos());
			trans->setScale(m_transform.getScale());
			trans->setRotation(m_transform.getRotation());
			trans->setFlipX(m_transform.getFlippedX());
			trans->setFlipY(m_transform.getFlippedY());
		}
	}

	//draws the particle to the screen
	void ObjParticle::draw(DrawInfo& drawInfo, const sf::FloatRect& viewBox) {
		//does nothing because the objects are already drawn through the usual scene
	};

	//returns a clone of the particle with the given transform info
	Particle* ObjParticle::clone(float lifeTime, Vector2 pos, float scale, float rotation) {
		ObjParticle* newParticle = new ObjParticle(lifeTime, pos, scale, rotation, m_prototype->clone(), SYNCPOSITION, ORIGINALCOLOR);
		newParticle->cloneComponentsFrom(this);
		return newParticle;
	};

	//returns a const pointer to the object the particle manages
	BasicParticleObj* ObjParticle::getObject() {
		return m_object;
	}

	//destructor
	ObjParticle::~ObjParticle() {
		if (m_object != nullptr) {
			m_object->m_objParticleOwner = nullptr;
			m_object->destroy();
		}
		//frees the object prototype
		delete m_prototype;
	}

}