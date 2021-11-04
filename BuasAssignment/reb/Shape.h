#ifndef REBOOTSHAPE_H
#define REBOOTSHAPE_H

#pragma once
#include <SFML/Graphics.hpp>
#include "Vector2.h"
#include "Rect.h"
#include "Material.h"
#include "Transform.h"
#include "Content.h"

namespace reb {

	class Rigidbody;

	//abstract base class for all Rigidbody shapes
	class Shape {
		friend Rigidbody;
	private:
		mutable Transform m_lastAABBTransform;
		mutable Rect<float> m_lastAABB;
		mutable bool m_initalizedAABB = false;
	protected:
		//the centor of mass of the shape
		Vector2 COM;
		//the area of the shape
		float area;

		//sets the area of the shape
		virtual void setArea() = 0;
		//sets the center of mass
		virtual void setCOM() = 0;
		//returns the local space inertia
		virtual float getLocalInertia(Material& mat) = 0;
		//returns the minimal enclosing AABB
		virtual Rect<float> getCurrentAABB(Transform& trans)const = 0;
	public:
		//returns the minimal enclosing AABB
		Rect<float> getAABB(Transform& trans)const;
		//updates the shape to match the given content
		virtual void updateShape(Content* content) = 0;
		//draws the shape to the screen (for debugging (usually))
		virtual void draw(sf::RenderTarget&, const Transform& trans, bool solid)const = 0;
		//returns a clone of the shape
		virtual Shape* clone()const = 0;

		//returns true if the given point (in local space) is within the shape
		virtual bool withinShape(Vector2 localPoint)const = 0;

		//returns the smallest distance from the given point to the shape
		virtual float distanceFrom(Vector2 localPoint)const = 0;

		//returns the inertia of the shape given the material
		float getIntertia(Material& mat, float scale);
		//returns the center of mass
		Vector2 getCOM()const;
		//returns the total mass of the shape using the density of the given material
		float getMass(Material& mat, float scale)const;

		//virtual destructor
		virtual ~Shape() {};
	};

}

#endif