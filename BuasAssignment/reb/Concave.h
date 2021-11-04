#ifndef REBOOTCONCAVE_H
#define REBOOTCONCAVE_H

#pragma once
#include "Convex.h"

namespace reb {

	//a concave is a combination of multiple convex shapes
	class Concave : public Shape {
	private:
		//all the convex shapes that make up the concave
		std::vector<Convex*> m_convexes;
		
		//keeps the corners of the aabb
		std::array<Vector2, 4> m_cornersAABB;

		//the currently set inertia of the shape
		float m_currentInertia;
		//the density the current inertia is based on
		float m_currentDensity;

		//sets the corners of the aabb
		void setupAABB();

		//returns the inertia of the shape given the material
		float getLocalInertia(Material& mat) override;

	protected:
		//sets the area of the shape
		void setArea() override;
		//sets the center of mass
		void setCOM() override;
		//returns the minimal enclosing AABB
		Rect<float> getCurrentAABB(Transform& trans)const override;
	public:
		//constructors
		Concave(std::vector<Convex*> convexes);
		Concave(std::vector<std::vector<Vector2>> convexes);
		Concave(Content* content);

		//copy constructor
		Concave(const Concave &obj);

		//returns a clone of the shape
		virtual Shape* clone()const override;

		//draws the shape to the screen (for debugging (usually))
		void draw(sf::RenderTarget&, const Transform& trans, bool solid)const override;
		//updates the shape to match the given content
		void updateShape(Content* content)override;

		//returns the smallest distance from the given point to the shape
		float distanceFrom(Vector2 localPoint)const override;

		//returns a const reference to the vector of concaves
		const std::vector<Convex*>& getConvexes()const;

		//returns true if the given point (in local space) is within the shape
		virtual bool withinShape(Vector2 localPoint)const override;

		//destructor
		~Concave();
	};

}

#endif