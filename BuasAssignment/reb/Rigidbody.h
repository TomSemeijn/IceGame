#ifndef REBOOTRIGIDBODY_H
#define REBOOTRIGIDBODY_H

#pragma once
#include "MainComponent.h"
#include "Shape.h"
#include "Material.h"
#include "Container.h"
#include "Functions.h"
#include "Content.h"

namespace reb {

	class Scene;

	//the Rigidbody is a shape of mass on which forces can be applied. In this definition, a Rigidbody can be
	//both solid and non-solid. When a solid Rigidbody collides with another, impulse resolution is used to
	//separate the two. When a non-solid Rigidbody collides with any other Rigidbody, the onCollide methods are
	//called on both Rigidbodies with the other as an argument
	class Rigidbody : public MainComponent {
		friend Scene;

		//defines container as a friend so it can set itself as owner
		friend Container;

	private:
		//the air resistance factor by which velocity will reduce after being applied
		const float AIRRESISTANCE = 0.99f;
		const float MINVELOCITY = 1;
		const float MINANGULARVELOCITY = MINVELOCITY * (PI / 180.0f);

		//a struct for holding information about the state of a trigger
		struct TriggerInfo {
			Rigidbody* trigger = nullptr;
			bool isInside = false;
			bool wasInside = false;

			bool operator==(const TriggerInfo& other) { return other.trigger == trigger; };
		};

		//a list of triggers the rigidbody is/was intersecting with
		std::vector<TriggerInfo> m_triggers;

		//the shape and material the body is based on
		Shape* m_shape;
		Material m_material;

		//general information based on the shape and material
		float m_inv_mass;
		float m_inv_inertia;
		Vector2 m_COM;

		//holds the scale of the last update to know when to update mass and inertia
		float oldScale;

		//the scale of gravity that affects this body
		Vector2 m_gravity;

		//current positional information
		Vector2 m_force;
		Vector2 m_velocity;
		//current rotational information
		float m_torque;
		float m_angularVelocity;


		//updates the general information using the shape and material
		void updateGeneralInfo();
		//resets force and torque
		void resetForces();

		//sets a trigger as isInside (adding it if necessary)
		void setInside(Rigidbody* trigger);
		//updates the triggers and calls the tirgger enter and leave functions if necessary
		void updateTriggers();

		//updates the bounds of the owning object if that's the rigidbody's job
		void updateBounds();

		//gets called when the owner of the component is set
		virtual void onOwnerSet()override;

		//copy constructor
		Rigidbody(const Rigidbody &obj);
	public:
		//if the Rigidbody is solid or not
		const bool SOLID;

		//constructor
		Rigidbody(Shape* shape, Material material, float gravity, bool solid);

		//returns a copy of the component
		virtual Rigidbody* clone()const override;
		
		//adds a force to the body
		void addForce(Vector2 force);
		//adds torque to the body
		void addTorque(float torque);

		//adds velocity to the body
		void addVelocity(Vector2 velocity);
		//adds angular velocity to the body
		void addAngularVelocity(float angular);

		//sets the velocity of the body
		void setVelocity(Vector2 newVelocty);
		//sets the angular velocity of the body
		void setAngularVelocity(float newAngular);

		//applies an impulse to the body
		void applyImpulse(Vector2 impulse, Vector2 contact, float elapsed);

		//return the current force
		Vector2 getForce()const;
		//returns the current velocity
		Vector2 getVelocity()const;
		//returns the current torque
		float getTorque()const;
		//returns the current angular velocity
		float getAngularVelocity()const;
		//returns the scale of gravity on this body
		Vector2 getGravity()const;

		//applies the forces of the body to the velocities
		void applyForces(float elapsed);
		//applies the velocities of the body to the position of the owner
		void applyVelocities(float elapsed);

		//updates the shape
		void updateShape(Shape* newShape);
		//updates the material
		void updateMaterial(Material newMaterial);
		//updates the shape and the material
		void updateBody(Shape* newShape, Material newMaterial);

		//returns a read-only pointer to the shape
		const Shape *const getShape()const;
		//returns the material
		Material getMaterial()const;

		//animates the shape of the rigidbody
		void animateShape(Content* content);

		//returns the inverse mass
		float getInverseMass();
		//returns the inverse inertia
		float getInverseInertia();

		//returns true if the given container is currently within the body as a trigger
		bool hasTrigger(Container*)const;

		//sets the gravity
		void setGravity(float newGravity);
		void setGravity(Vector2 newGravity);

		//removes the rigidbody from the owning object
		void destroy()override;

		//destructor
		virtual ~Rigidbody();
	};

}

#endif