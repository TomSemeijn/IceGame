#ifndef REBOOTCONVEX_H
#define REBOOTCONVEX_H

#pragma once
#include <array>
#include "Shape.h"
#include "Content.h"
#include "Color.h"
#include "Rect.h"

namespace reb {

	class Concave;

	//the convex shape
	class Convex : public Shape {
		friend Concave;
	private:
		//a vector of vertices that make up the polygon
		std::vector<Vector2> m_vertices;
		mutable std::vector<Vector2> m_constVertices;
		//a vector of normals of every edge of the polygon
		std::vector<Vector2> m_normals;

		//keeps the aabb corners so they can be used to generate an aabb instead of all the vertices
		std::array<Vector2, 4> m_cornersAABB;

		//keeps the local space aabb
		Rect<float> m_localAABB;

		//the currently set inertia of the shape
		float m_currentInertia;
		//the density the current inertia is based on
		float m_currentDensity;


		//the color to draw the shape with
		const Color EVENDRAWCOLOR{ 255, 0, 0 };
		const Color ODDDRAWCOLOR{ 255, 255, 255 };

		//sets up the aabb corners
		void setupAABB();
		//returns the signed area of the shape (using the formula from https://en.wikipedia.org/wiki/Centroid#Of_a_polygon)
		float getSignedArea()const;
		//sets the normals of the polygon
		void setNormals();

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
		//constructor
		Convex(std::vector<Vector2> vertices);
		Convex(Rect<float> rect);
		Convex(Content* content);

		//returns a clone of the shape
		virtual Shape* clone()const override;

		//draws the shape to the screen (for debugging (usually))
		void draw(sf::RenderTarget&, const Transform& trans, bool solid)const override;
		//updates the shape to match the given content (takes a local space AABB around the vertices)
		void updateShape(Content* content)override;

		//returns a constant reference to the vector of vertices
		const std::vector<Vector2>& getVertices()const;
		//returns a constant reference to the vector of normals
		const std::vector<Vector2>& getNormals()const;

		//returns true if the given point (in local space) is within the shape
		virtual bool withinShape(Vector2 localPoint)const override;

		//returns the smallest distance from the given point to the shape
		float distanceFrom(Vector2 localPoint)const override;

		//returns true if the given point (in local space) is within the shape (faster- but less accurate -version)
		bool withinShapeFaster(Vector2 localPoint)const;

		//returns true if the given AABB intersects with the convex
		bool intersectsAABB(Rect<float>& aabb, Transform& trans)const;
	};

}

#endif