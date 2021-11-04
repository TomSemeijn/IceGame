#ifndef REBOOTCIRCLE_H
#define REBOOTCIRCLE_H

#pragma once
#include "Shape.h"
#include "Content.h"
#include "Color.h"

namespace reb {

	//the circle shape (pretty straightforward honestly)
	class Circle : public Shape {
	protected:
		//the center of the circle
		Vector2 m_center;
		//the radius of the circle
		float m_radius;

		//the color to draw the shape with
		const Color DRAWCOLOR{ 255, 255, 255 };

		//sets the area of the shape
		void setArea() override;
		//sets the center of mass
		void setCOM() override;

		//sets the circle to be a minimum enclosing circle of the given vector of vertices
		void setMinimumEnclosing(std::vector<Vector2> vertices);

		//returns the inertia of the shape given the material
		float getLocalInertia(Material& mat) override;
		//returns the minimal enclosing AABB
		Rect<float> getCurrentAABB(Transform& trans)const override;
	public:
		//constructor
		Circle(Vector2 center, float radius) : m_center{ center }, m_radius{ radius }{ setArea(); setCOM(); };
		//constructor using a vector of vertices to enclose
		Circle(std::vector<Vector2> vertices) { setMinimumEnclosing(vertices); setArea(); setCOM(); };
		//constructor using a shared pointer to a content object
		Circle(Content* content);

		//returns a clone of the shape
		Shape* clone()const override;

		//draws the shape to the screen (for debugging (usually))
		void draw(sf::RenderTarget&, const Transform& trans, bool solid)const override;
		//updates the shape to match the given content
		void updateShape(Content* content)override;

		//returns the smallest distance from the given point to the shape
		float distanceFrom(Vector2 localPoint)const override;

		//returns true if the given point (in local space) is within the shape
		bool withinShape(Vector2 localPoint)const override;

		//returns the center
		Vector2 getCenter()const;
		//returns the radius
		float getRadius()const;

	};

}

#endif