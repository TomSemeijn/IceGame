#include "Convex.h"

#include "Functions.h"

namespace reb {

	//constructor
	Convex::Convex(std::vector<Vector2> vertices) : m_vertices{ vertices } { setArea(); setCOM(); setNormals(); setupAABB(); };
	Convex::Convex(Rect<float> rect) :
		m_vertices{
			{
				Vector2(rect.left, rect.top),
				Vector2(rect.left + rect.width, rect.top),
				Vector2(rect.left + rect.width, rect.top + rect.height),
				Vector2(rect.left, rect.top + rect.height)
			}
	}
	{
		setArea(); setCOM(); setNormals(); setupAABB();
	};
	Convex::Convex(Content* content) {
		updateShape(content);
	}

	//returns a clone of the shape
	Shape* Convex::clone()const {
		return new Convex(*this);
	}

	//sets up the aabb corners
	void Convex::setupAABB() {
		//gets the bounds of the Convex
		float left = m_vertices.front().x;
		float right = left;
		float top = m_vertices.front().y;
		float bottom = top;
		for (auto& vert : m_vertices) {
			left = std::min(left, vert.x);
			right = std::max(right, vert.x);
			top = std::min(top, vert.y);
			bottom = std::max(bottom, vert.y);
		}
		//sets the corner values
		m_cornersAABB[0] = Vector2(left, top);
		m_cornersAABB[1] = Vector2(right, top);
		m_cornersAABB[2] = Vector2(right, bottom);
		m_cornersAABB[3] = Vector2(left, bottom);
		//sets the local aabb
		m_localAABB = Rect<float>(left, top, right - left, bottom - top);
	}

	//sets the normals of the Convex
	void Convex::setNormals() {
		m_normals.clear();
		//loops through all vertices
		for (auto vert = m_vertices.begin(); vert < m_vertices.end(); vert++) {
			//gets next vertex
			auto next = (vert + 1 < m_vertices.end() ? vert + 1 : m_vertices.begin());
			//gets edge vector
			Vector2 edge = *next - *vert;
			//add normal
			m_normals.push_back(Vector2(edge.y, -edge.x).setLength(1));
		}
		m_constVertices = m_vertices;
	}

	//returns a constant reference to the vector of vertices
	const std::vector<Vector2>& Convex::getVertices()const {
		return m_vertices;
	}

	//returns a constant reference to the vector of normals
	const std::vector<Vector2>& Convex::getNormals()const {
		return m_normals;
	}

	//returns the area of the shape
	void Convex::setArea() {
		//returns the absolute value of the signed area
		area = std::abs(getSignedArea());
	}

	//returns the inertia (using https://en.wikipedia.org/wiki/List_of_moments_of_inertia)
	float Convex::getLocalInertia(Material& mat) {
		//checks if the inertia should be updated
		if (mat.density != m_currentDensity) {
			//keeps a value for 1 / 3
			float third = 1.0f / 3.0f;

			//initializes value for the numerator and denominator of the inertia formula for a plane Convex
			float numerator = 0;
			float denominator = 0;
			//loops through all vertices
			for (auto vert = m_vertices.begin(); vert < m_vertices.end(); vert++) {
				//gets the vertex after the current one
				auto next = (vert + 1 < m_vertices.end() ? vert + 1 : m_vertices.begin());

				//adds to the numerator
				numerator += next->crossProd(*vert) * vert->dotProduct(*vert) + vert->dotProduct(*next) + next->dotProduct(*vert);

				//adds to the denominator
				denominator += next->crossProd(*vert);
			}
			//gets final inertia value
			float inertia = numerator / (denominator * 6.0f);
			//sets the current inertia to the found interia value multiplied by the mass of the material
			m_currentInertia = inertia * mat.density * area;
			m_currentDensity = mat.density;
		}
		//returns the current inertia
		return m_currentInertia;
	}

	//returns the signed area of the shape (using the formula from https://en.wikipedia.org/wiki/Centroid#Of_a_Convex)
	float Convex::getSignedArea()const {
		//initializes an area value
		float area = 0;
		//loops through all vertices
		for (auto vert = m_vertices.begin(); vert < m_vertices.end(); vert++) {
			//gets the vertex after the current one
			auto next = (vert + 1 < m_vertices.end() ? vert + 1 : m_vertices.begin());
			//adds the cross product of the current vertex and the next to the area
			area += vert->crossProd(*next);
		}
		//returns the found area divided by two
		return area * 0.5f;
	}

	//returns the center of mass (using the formula from https://en.wikipedia.org/wiki/Centroid#Of_a_Convex)
	void Convex::setCOM() {
		//initializes a value for an xSum and a ySum
		float xSum = 0;
		float ySum = 0;
		//loops through all vertices
		for (auto vert = m_vertices.begin(); vert < m_vertices.end(); vert++) {
			//gets the vertex after the current one
			auto next = (vert + 1 < m_vertices.end() ? vert + 1 : m_vertices.begin());
			//adds to the xSum and ySum values
			xSum += (vert->x + next->x) * vert->crossProd(*next);
			ySum += (vert->y + next->y) * vert->crossProd(*next);
		}
		//gets 1 / (signedArea * 6)
		float scalar = 1.0f / (getSignedArea() * 6.0f);
		//returns final center of mass
		COM = Vector2(scalar * xSum, scalar * ySum);
	}

	//returns the minimal enclosing AABB
	Rect<float> Convex::getCurrentAABB(Transform& trans)const {
		//copies the aabb corners
		std::array<Vector2, 4> cornerCopy{ m_cornersAABB };
		Vector2 worldCOM = trans.getPos() + getCOM();
		//sets the corners to world space
		for (int k = 0; k < 4; k++) {
			cornerCopy[k] = trans.getWorldSpace(cornerCopy[k]);
		}
		//gets the bounds of the corners
		float left = cornerCopy[0].x;
		float right = cornerCopy[0].x;
		float top = cornerCopy[0].y;
		float bottom = cornerCopy[0].y;
		for (int k = 1; k < 4; k++) {
			left = std::min(left, cornerCopy[k].x);
			right = std::max(right, cornerCopy[k].x);
			top = std::min(top, cornerCopy[k].y);
			bottom = std::max(bottom, cornerCopy[k].y);
		}
		//returns the rectangle made of the found bounds
		return Rect<float>(left, top, right - left, bottom - top);
	}

	//draws the Rigidbody to the screen (for debugging (usually))
	void Convex::draw(sf::RenderTarget& window, const Transform& trans, bool solid)const {
		//gets center of mass in world space
		Vector2 COM = trans.getWorldSpace(getCOM());
		//creates a vertexArray
		sf::VertexArray toDraw{ sf::PrimitiveType::Lines, m_vertices.size() * 2 };
		//sets up drawColor
		sf::Color sfmlEvenColor = EVENDRAWCOLOR.getSfml();
		sf::Color sfmlOddColor = ODDDRAWCOLOR.getSfml();
		//adds every vertex
		for (int i = 0; i < m_vertices.size(); i++) {
			int index = i * 2;

			Vector2 vert = trans.getWorldSpace(m_vertices[i]);
			toDraw[index] = vert.getSfml();
			toDraw[index].color = sfmlEvenColor;

			Vector2 vert2 = trans.getWorldSpace(m_vertices[(i + 1 < m_vertices.size() ? i + 1 : 0)]);
			toDraw[index + 1] = vert2.getSfml();
			toDraw[index + 1].color = sfmlOddColor;
		}
		
		//draws the aabb
		/*
		auto aabb = getAABB(trans);
		sf::RectangleShape aabbDraw{ sf::Vector2f(aabb.width, aabb.height) };
		aabbDraw.setPosition(sf::Vector2f(aabb.left, aabb.top));
		aabbDraw.setFillColor(sf::Color(255, 0, 0, 120));
		window.draw(aabbDraw);*/
		

		//draws the normals
		sf::VertexArray normals{ sf::PrimitiveType::Lines, (m_vertices.size() + 1) * 2 };
		for (int i = 0; i < m_vertices.size(); i++) {
			int index = i * 2;

			Vector2 edge1 = m_vertices[i];
			Vector2 edge2 = m_vertices[(i + 1 < m_vertices.size() ? i + 1 : 0)];
			Vector2 world1 = trans.getWorldSpace(edge1);
			Vector2 world2 = trans.getWorldSpace(edge2);

			Vector2 normalPos = world1 + (world2 - world1) * 0.5f;
			Vector2 normalDir = trans.getWorldNormal(m_normals[i]);

			normals[index] = normalPos.getSfml();
			normals[index + 1] = (normalPos + normalDir.setLength(2)).getSfml();

		}
	
		//draws center of mass
		sf::VertexArray drawCOM{ sf::PrimitiveType::Points, 1 };
		drawCOM[0] = COM.getSfml();
		drawCOM[0].color = sf::Color{ 255, 255, 0 };
		window.draw(drawCOM);
		//draws vertexArray
		window.draw(toDraw);
		window.draw(normals);
	}

	//returns true if the given AABB intersects with the convex
	bool Convex::intersectsAABB(Rect<float>& aabb, Transform& trans)const {
		//returns true if any of the vertices are within the aabb
		Vector2 pos = trans.getPos();
		for (auto& ogVert : m_vertices) {
			if (aabb.withinRect(trans.getWorldSpace(ogVert))) { return true; }
		}
		//returns true if any of the aabb's corners is within the convex
		Vector2 topLeft = Vector2(aabb.left, aabb.top) - pos;
		Vector2 topRight = Vector2(aabb.left + aabb.width, aabb.top) - pos;
		Vector2 bottomLeft = Vector2(aabb.left, aabb.top + aabb.height) - pos;
		Vector2 bottomRight = Vector2(aabb.left + aabb.width, aabb.top + aabb.height) - pos;
		if (withinShapeFaster(topLeft)) { return true; }
		if (withinShapeFaster(topRight)) { return true; }
		if (withinShapeFaster(bottomLeft)) { return true; }
		if (withinShapeFaster(bottomRight)) { return true; }
		//returns false because all tests failed
		return false;
	}

	//updates the shape to match the given content (takes a local space AABB around the vertices)
	void Convex::updateShape(Content* content) {
		//finds the bounds
		float left, right, top, bottom;
		bool started = false;
		for (auto& poly : content->getConvexes()) {
			for (auto& vert : poly) {
				if (!started) {
					left = vert.x;
					right = vert.x;
					top = vert.y;
					bottom = vert.y;
					started = true;
				}
				else {
					left = std::min(left, vert.x);
					right = std::max(right, vert.x);
					top = std::min(top, vert.y);
					bottom = std::max(bottom, vert.y);
				}
			}
		}
		//sets the vertices of the convex to match the found bounds
		m_vertices.clear();
		m_vertices.emplace_back(left, top);
		m_vertices.emplace_back(right, top);
		m_vertices.emplace_back(right, bottom);
		m_vertices.emplace_back(left, bottom);
		//sets all the important values
		setArea(); setCOM(); setNormals(); setupAABB();
	}

	//returns true if the given point (in local space) is within the shape
	bool Convex::withinShape(Vector2 localPoint)const {
		m_constVertices.push_back(m_constVertices.front());
		Vector2 pointRight = localPoint + Vector2(100000, 0);
		std::vector<Vector2> intersects{};
		intersects.reserve(m_constVertices.size());
		auto vertEnd = m_constVertices.end() - 1;
		for (auto first = m_constVertices.begin(); first < vertEnd; first++) {
			auto next = first + 1;
			Vector2 intersection{};
			if (linesIntersectAt(*first, *next, localPoint, pointRight, intersection)) {
				if (!withinVector(intersects, intersection)) {
					intersects.push_back(intersection);
				}
			}
		}
		m_constVertices.pop_back();
		return intersects.size() % 2 != 0;
	}

	//returns true if the given point (in local space) is within the shape
	bool Convex::withinShapeFaster(Vector2 localPoint)const {
		m_constVertices.push_back(m_vertices.front());
		Vector2 pointRight = localPoint + Vector2(100000, 0);
		int intersects = 0;
		auto vertEnd = m_constVertices.end() - 1;
		for (auto first = m_constVertices.begin(); first < vertEnd; first++) {
			auto next = first + 1;
			if (linesIntersect(*first, *next, localPoint, pointRight)) { intersects++; }
		}
		m_constVertices.pop_back();
		return intersects % 2 != 0;
	}

	//returns the smallest distance from the given point to the shape
	float Convex::distanceFrom(Vector2 localPoint)const {
		//loops through all the edges
		float minDistance = std::numeric_limits<float>::max();
		m_constVertices.push_back(m_constVertices.front());
		auto vertEnd = m_constVertices.end() - 1;
		for (auto vert = m_constVertices.begin(); vert < vertEnd; vert++) {
			auto next = vert + 1;
			//updates the distance if the current distance is lower
			minDistance = std::fminf(minDistance, distancePointLine(*vert, *next, localPoint));
		}
		m_constVertices.pop_back();
		//returns the lowest distance found
		return minDistance;
	}

}