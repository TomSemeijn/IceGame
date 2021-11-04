#include "Rigidbody.h"

namespace reb {

	//constructor
	Rigidbody::Rigidbody(Shape* shape, Material material, float gravity, bool solid)
		: SOLID{ solid },
		m_shape{ shape },
		m_material{ material },
		m_gravity{ 0, gravity },
		m_COM{ m_shape->getCOM() }
	{}

	//returns a copy of the component
	Rigidbody* Rigidbody::clone()const {
		return new Rigidbody(*this);
	}

	//copy constructor
	Rigidbody::Rigidbody(const Rigidbody &obj) : 
		SOLID{ obj.SOLID }, 
		m_shape{ obj.getShape()->clone() }, 
		m_material{ obj.getMaterial() },
		m_gravity{ obj.getGravity() },
		m_COM{ obj.m_COM }
	{}

	//adds velocity to the body
	void Rigidbody::addVelocity(Vector2 velocity) {
		m_velocity += velocity;
	}

	//adds angular velocity to the body
	void Rigidbody::addAngularVelocity(float angular) {
		m_angularVelocity += angular;
	}

	//sets the velocity of the body
	void Rigidbody::setVelocity(Vector2 newVelocty) {
		m_velocity = newVelocty;
	}

	//sets the angular velocity of the body
	void Rigidbody::setAngularVelocity(float newAngular) {
		m_angularVelocity = newAngular;
	}

	//gets called when the owner of the component is set
	void Rigidbody::onOwnerSet() {
		if (getOwner() != nullptr) {
			updateGeneralInfo();
			updateBounds();
			getOwner()->getTransform()->setCenter(m_shape->getCOM());
		}
	}

	//returns a read-only pointer to the shape
	const Shape *const Rigidbody::getShape()const {
		return m_shape;
	}

	//returns the material
	Material Rigidbody::getMaterial()const {
		return m_material;
	}

	//returns the inverse mass
	float Rigidbody::getInverseMass() {
		if (getOwner()->getTransform()->getScale() != oldScale) { updateGeneralInfo(); }
		return m_inv_mass;
	}

	//returns the inverse inertia
	float Rigidbody::getInverseInertia() {
		if (getOwner()->getTransform()->getScale() != oldScale) { updateGeneralInfo(); }
		return m_inv_inertia;
	}

	//returns the scale of gravity on this body
	Vector2 Rigidbody::getGravity()const {
		return m_gravity;
	}

	//adds a force to the body
	void Rigidbody::addForce(Vector2 force) {
		m_force += force;
	}

	//applies an impulse to the body
	void Rigidbody::applyImpulse(Vector2 impulse, Vector2 contact, float elapsed) {
		//gets the values to add to the velocities
		Vector2 velocityToAdd = impulse * getInverseMass();
		float angularToAdd = contact.crossProd(impulse) * getInverseInertia();

		//adds values to velocities
		m_velocity += velocityToAdd;
		m_angularVelocity += angularToAdd;

		//writeLn("impulse of ", impulse, " applied to ", typeid(*m_shape).name());

		//moves owning object according to what was added
		//m_owner->m_transform.move(velocityToAdd * elapsed);
		//m_owner->m_transform.rotate(angularToAdd * elapsed);
	}

	//adds torque to the body
	void Rigidbody::addTorque(float torque) {
		m_torque += torque;
	}

	//return the current force
	Vector2 Rigidbody::getForce()const {
		return m_force;
	}

	//returns the current velocity
	Vector2 Rigidbody::getVelocity()const {
		return m_velocity;
	}

	//returns the current torque
	float Rigidbody::getTorque()const {
		return m_torque;
	}

	//returns the current angular velocity
	float Rigidbody::getAngularVelocity()const {
		return m_angularVelocity;
	}

	//applies the forces of the body to the velocities
	void Rigidbody::applyForces(float elapsed) {
		//gets the differences in velocities
		Vector2 addVelocity = (m_force * getInverseMass() + m_gravity) * elapsed;
		float addAngular = m_torque * getInverseInertia() * elapsed;

		//adds the differences to the velocities
		m_velocity += addVelocity;
		m_angularVelocity += addAngular;

		//resets the forces
		resetForces();
	}

	//applies the velocities of the body to the position of the owner
	void Rigidbody::applyVelocities(float elapsed) {
		if (m_velocity.getLength() > MINVELOCITY) {
			auto old = getOwner()->getTransform()->getPos();
			getOwner()->getTransform()->move(m_velocity * elapsed);
			//applies air resistance to slow down the velocities
			m_velocity *= AIRRESISTANCE;
		}
		if (std::abs(m_angularVelocity) > MINANGULARVELOCITY) {
			getOwner()->getTransform()->rotate(m_angularVelocity * elapsed);
			m_angularVelocity *= AIRRESISTANCE;
		}
	}

	//resets force and torque
	void Rigidbody::resetForces() {
		m_force = Vector2();
		m_torque = 0;
	}

	//updates the general information using the shape and material
	void Rigidbody::updateGeneralInfo() {

		//sets old scale
		oldScale = getOwner()->getTransform()->getScale();
		
		//sets inverse mass
		if (getOwner()->getTransform()->MOVABLE) {
			//sets to actual value if the owning object is movable
			float mass = m_shape->getMass(m_material, getOwner()->getTransform()->getScale());
			//prevents division by zero
			if (mass) {
				m_inv_mass = 1.0f / mass;
			}
			else {
				m_inv_mass = 0;
			}
		}//sets to zero (for infinite mass) if the owning object is not movable
		else {
			m_inv_mass = 0;
		}
		
		//sets inverse inertia
		if (getOwner()->getTransform()->ROTATABLE) {
			//sets to actual value if the owning object is rotatable
			float inertia = m_shape->getIntertia(m_material, getOwner()->getTransform()->getScale());
			//prevents division by zero
			if (inertia) {
				m_inv_inertia = 1.0f / inertia;
			}
			else {
				m_inv_inertia = 0;
			}
		}//sets to zero (for infinite inertia) if the owning object is not rotatable
		else {
			m_inv_inertia = 0;
		}
		
	}

	//updates the shape
	void Rigidbody::updateShape(Shape* newShape) {
		delete m_shape;
		m_shape = newShape;
	}

	//updates the material
	void Rigidbody::updateMaterial(Material newMaterial) {
		m_material = newMaterial;
	}

	//updates the shape and the material
	void Rigidbody::updateBody(Shape* newShape, Material newMaterial) {
		delete m_shape;
		m_shape = newShape;
		m_material = newMaterial;
	}

	//animates the shape of the rigidbody
	void Rigidbody::animateShape(Content* content) {
		//changes the shape
		m_shape->updateShape(content);
		//resets the center of the object in case the center of mass changed
		getOwner()->getTransform()->setCenter(m_shape->getCOM());
		updateBounds();
	}

	//updates the bounds of the owning object if that's the rigidbody's job
	void Rigidbody::updateBounds() {
		//checks if the rigidbody should update the object bounds
		if (getOwner()->BOUNDTYPE == Container::BOUNDS_RIGIDBODY) {
			//sets the bounds as the aabb of the shape with a transform at the origin
			Transform temp{};
			getOwner()->getTransform()->setBounds(m_shape->getAABB(temp));
		}
	}

	//sets a trigger as isInside (adding it if necessary)
	void Rigidbody::setInside(Rigidbody* trigger) {
		//tries to find the trigger within the list
		auto triggerIter = std::find_if(m_triggers.begin(), m_triggers.end(), [&trigger](TriggerInfo& element) {
			return element.trigger == trigger;
		});
		//sets to isInside if it was found
		if (triggerIter != m_triggers.end()) {
			triggerIter->isInside = true;
		}
		//adds triggerInfo with isInside as true if it was not found
		else {
			TriggerInfo newInfo{};
			newInfo.trigger = trigger;
			newInfo.isInside = true;
			m_triggers.push_back(newInfo);
		}
	}

	//updates the triggers and calls the tirgger enter and leave functions if necessary
	void Rigidbody::updateTriggers() {
		//keeps a vector of trigger indexes to remove
		std::vector<TriggerInfo> toRemove{};
		//iterates through the triggers
		for (auto trigger = m_triggers.begin(); trigger < m_triggers.end(); trigger++) {
			if (trigger->trigger == nullptr) {
				toRemove.push_back(*trigger);
			}
			else {
				//if the trigger just entered the body
				if (trigger->isInside && !trigger->wasInside) {
					//calls triggerEnter
					getOwner()->triggerEnter(trigger->trigger->getOwner());
				}
				//if the trigger left the body
				else if (trigger->wasInside && !trigger->isInside) {
					//calls triggerLeave
					getOwner()->triggerLeave(trigger->trigger->getOwner());
					//adds the trigger to toRemove
					toRemove.push_back(*trigger);
				}
				//updates the booleans
				trigger->wasInside = trigger->isInside;
				trigger->isInside = false;
			}
		}
		//removes all triggers that left the rigidbody
		if (!toRemove.empty()) {
			for (auto& rem : toRemove) {
				swapPopEl(m_triggers, rem);
			}
		}
	}

	//returns true if the given container is currently within the body as a trigger
	bool Rigidbody::hasTrigger(Container* other)const {
		return	std::find_if(m_triggers.begin(), m_triggers.end(),
						[&other](const TriggerInfo& trigger) {
							return trigger.trigger->hasOwner() && trigger.trigger->getOwner() == other && (trigger.isInside || trigger.wasInside); 
						}
				) != m_triggers.end();
	}

	//sets the gravity
	void Rigidbody::setGravity(float newGravity) {
		m_gravity = Vector2(0, newGravity);
	}

	void Rigidbody::setGravity(Vector2 newGravity) {
		m_gravity = newGravity;
	}

	//removes the rigidbody from the owning object
	void Rigidbody::destroy() {
		if (hasOwner()) {
			getOwner()->clearBody();
		}
	}

	//destructor
	Rigidbody::~Rigidbody() {
		delete m_shape;
	}

}