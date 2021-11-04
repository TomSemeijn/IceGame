#include "Circle.h"
#include <array>
#include "Miniball.h"
#include "Functions.h"

namespace reb {

	//constructor using a shared pointer to a content object
	Circle::Circle(Content* content) {
		updateShape(content);
	}

	//returns a clone of the shape
	Shape* Circle::clone()const {
		return new Circle(*this);
	}

	//updates the shape to match the given content
	void Circle::updateShape(Content* content) {
		//puts all convex vertices into one vector
		std::vector<Vector2> vertices;
		for (auto& convex : content->getConvexes()) {
			for (auto& vert : convex) {
				vertices.push_back(vert);
			}
		}
		//sets circle using miniball
		setMinimumEnclosing(vertices);
		//sets area and center of mass
		setArea();
		setCOM();
	}

	//sets the circle to be a minimum enclosing circle of the given vector of vertices
	void Circle::setMinimumEnclosing(std::vector<Vector2> vertices) {
		//sets up miniball typedefs
		typedef std::vector<std::array<float, 2>>::const_iterator pointIterator;
		typedef std::array<float, 2>::const_iterator coordIterator;
		typedef Miniball::Miniball<Miniball::CoordAccessor<pointIterator, coordIterator>> miniB;
		//creates vector of two value arrays to put vertices in
		std::vector<std::array<float, 2>> verticesArr(vertices.size());
		//puts vertices into vector
		int current = 0;
		for (auto& vert : vertices) {
			verticesArr[current][0] = vert.x;
			verticesArr[current][1] = vert.y;
			current++;
		}
		//creates miniball
		miniB ball{ 2, verticesArr.begin(), verticesArr.end() };
		//gets values from miniball
		m_center = Vector2(ball.center()[0], ball.center()[1]);
		m_radius = std::powf(ball.squared_radius(), 0.5);
	}

	//returns the center
	Vector2 Circle::getCenter()const {
		return m_center;
	}

	//returns the radius
	float Circle::getRadius()const {
		return m_radius;
	}

	//returns the area of the shape
	void Circle::setArea() {
		area = 3.14159f * m_radius * m_radius;
	}

	//returns the center of mass
	void Circle::setCOM() {
		COM = m_center;
	}

	//returns the inertia
	float Circle::getLocalInertia(Material& mat) {
		//returns the mass multiplied by the radius squared
		return area * mat.density * m_radius * m_radius;
	}

	//returns the minimal enclosing AABB
	Rect<float> Circle::getCurrentAABB(Transform& trans)const {
		float scaledRadius = m_radius * trans.getScale();
		Vector2 worldCenter = trans.getWorldSpace(m_center);
		return Rect<float>(worldCenter.x - scaledRadius, worldCenter.y - scaledRadius, scaledRadius * 2, scaledRadius * 2);
	}

	//draws the Rigidbody to the screen (for debugging (usually))
	void Circle::draw(sf::RenderTarget& window, const Transform& trans, bool solid)const {
		//creates an sfml circle shape
		sf::CircleShape circle{ m_radius * trans.getScale() };
		//sets the position
		circle.setPosition((trans.getWorldSpace(m_center) - Vector2(m_radius, m_radius) * trans.getScale()).getSfml());
		//sets the color of the shape
		sf::Color sfmlColor = DRAWCOLOR.getSfml();
		circle.setFillColor(sf::Color(0, 0, 0, 0));
		circle.setOutlineColor(sfmlColor);
		circle.setOutlineThickness(1);
		
		//adds diagonals
		sf::VertexArray corners{ sf::PrimitiveType::Lines, 4 };
		Vector2 corner1 = m_center + Vector2(-1, -1).setLength(m_radius);
		Vector2 corner2 = m_center + Vector2(1, 1).setLength(m_radius);
		Vector2 corner3 = m_center + Vector2(-1, 1).setLength(m_radius);
		Vector2 corner4 = m_center + Vector2(1, -1).setLength(m_radius);
		corners[0] = sf::Vertex(trans.getWorldSpace(corner1).getSfml());
		corners[1] = sf::Vertex(trans.getWorldSpace(corner2).getSfml());
		corners[2] = sf::Vertex(trans.getWorldSpace(corner3).getSfml());
		corners[3] = sf::Vertex(trans.getWorldSpace(corner4).getSfml());
		corners[0].color = sfmlColor;
		corners[1].color = sfmlColor;
		corners[2].color = sfmlColor;
		corners[3].color = sfmlColor;
		window.draw(corners);

		//draws the shape
		window.draw(circle);
	}

	//returns the smallest distance from the given point to the shape
	float Circle::distanceFrom(Vector2 localPoint)const {
		return (localPoint - m_center).getLength() - m_radius;
	}

	//returns true if the given point (in local space) is within the shape
	bool Circle::withinShape(Vector2 localPoint)const {
		return (m_center - localPoint).getLength() <= m_radius;
	}

}