#include "Concave.h"
#include "Functions.h"

namespace reb {

	//constructor taking a vector of convexes
	Concave::Concave(std::vector<Convex*> convexes) : m_convexes{ convexes } {
		setArea();
		setCOM();
		setupAABB();
	}

	//constructor taking a vector of vectors of vertices (assumed to be convexes)
	Concave::Concave(std::vector<std::vector<Vector2>> convexes) {
		//loops through each set of vertices
		for (auto& conv : convexes) {
			//adds a convex
			Convex* currentConvex = new Convex{ conv };
			m_convexes.push_back(currentConvex);
			currentConvex->setNormals();
		}
		setArea();
		setCOM();
		setupAABB();
	}

	//constructor taking a shared pointer to a content object
	Concave::Concave(Content* content) {
		updateShape(content);
	}

	//copy constructor
	Concave::Concave(const Concave &obj) {
		auto convexes = obj.getConvexes();
		for (auto& conv : convexes) {
			m_convexes.push_back((Convex*)conv->clone());
		}
		setArea();
		setCOM();
		setupAABB();
	}

	//returns a clone of the shape
	Shape* Concave::clone()const {
		return new Concave(*this);
	}

	//updates the shape to match the given content
	void Concave::updateShape(Content* content) {
		//frees memory and clears the convexes
		for (auto& conv : m_convexes) { delete conv; }
		m_convexes.clear();
		//loops through each set of vertices
		for (auto& conv : content->getConvexes()) {
			//adds a convex
			Convex* currentConvex = new Convex{ conv };
			m_convexes.push_back(currentConvex);
			currentConvex->setNormals();
		}
		setArea();
		setCOM();
		setupAABB();
	}

	//sets the corners of the aabb
	void Concave::setupAABB() {
		//gets the bounds of the aabb's of the convexes
		float left = 0;
		float right = 0;
		float top = 0;
		float bottom = 0;
		bool started = false;
		Transform temp{ };
		for (auto& conv : m_convexes) {
			auto currentAABB = conv->getAABB(temp);
			if (!started) {
				left = currentAABB.left;
				right = currentAABB.left + currentAABB.width;
				top = currentAABB.top;
				bottom = currentAABB.top + currentAABB.height;
				started = true;
			}
			else {
				left = std::min(left, currentAABB.left);
				right = std::max(right, currentAABB.left + currentAABB.width);
				top = std::min(top, currentAABB.top);
				bottom = std::max(bottom, currentAABB.top + currentAABB.height);
			}
		}

		//sets the corner values
		m_cornersAABB[0] = Vector2(left, top);
		m_cornersAABB[1] = Vector2(right, top);
		m_cornersAABB[2] = Vector2(right, bottom);
		m_cornersAABB[3] = Vector2(left, bottom);

	}

	//sets the area of the shape
	void Concave::setArea() {
		//sets the area as the sum of the areas of all the convexes
		area = 0;
		for (auto& conv : m_convexes) { area += conv->area; }
	}

	//sets the center of mass
	void Concave::setCOM() {
		//sets the center of mass as the average of the centers of mass of all the convexes
		COM = Vector2();
		for (auto& conv : m_convexes) { COM += conv->COM * conv->area; }
		COM /= area;
		//sets this center of mass as the center of mass of all the convexes
		for (auto& conv : m_convexes) { conv->COM = COM; }
	}

	//returns the minimal enclosing AABB
	Rect<float> Concave::getCurrentAABB(Transform& trans)const {
		//copies the aabb corners
		std::array<Vector2, 4> cornerCopy{ m_cornersAABB };
		Vector2 worldCOM = trans.getWorldSpace(getCOM());
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

	//returns the inertia of the shape given the material
	float Concave::getLocalInertia(Material& mat) {
		//checks if the current inertia should be updated
		if (m_currentDensity != mat.density) {
			//gets a vector of unique vertices within the concave
			std::vector<Vector2> vertices;
			for (auto& conv : m_convexes) {
				for (auto& vert : conv->m_vertices) {
					if (std::find(vertices.begin(), vertices.end(), vert) == vertices.end()) {
						vertices.push_back(vert);
					}
				}
			}
			//does the same calculation for the inertia as a convex does using the found vertices
			{
				//keeps a value for 1 / 3
				float third = 1.0f / 3.0f;

				//initializes value for the numerator and denominator of the inertia formula for a plane Convex
				float numerator = 0;
				float denominator = 0;
				//loops through all vertices
				for (auto vert = vertices.begin(); vert < vertices.end(); vert++) {
					//gets the vertex after the current one
					auto next = (vert + 1 < vertices.end() ? vert + 1 : vertices.begin());

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
		}
		//returns the current inertia
		return m_currentInertia;
	}

	//draws the shape to the screen (for debugging (usually))
	void Concave::draw(sf::RenderTarget& window, const Transform& trans, bool solid)const {
		//draws every convex
		for (auto& conv : m_convexes) {
			conv->draw(window, trans, solid);
		}
		//draws the aabb
		/*auto aabb = getAABB(trans);
		sf::RectangleShape aabbDraw{ sf::Vector2f(aabb.width, aabb.height) };
		aabbDraw.setPosition(sf::Vector2f(aabb.left, aabb.top));
		aabbDraw.setFillColor(sf::Color(255, 0, 0, 120));
		window.draw(aabbDraw);*/
	}

	//returns a const reference to the vector of concaves
	const std::vector<Convex*>& Concave::getConvexes()const {
		return m_convexes;
	}

	//returns true if the given point (in local space) is within the shape
	bool Concave::withinShape(Vector2 localPoint)const {
		for (auto& conv : m_convexes) {
			if (conv->withinShape(localPoint)) { return true; }
		}
		return false;
	}

	//returns the smallest distance from the given point to the shape
	float Concave::distanceFrom(Vector2 localPoint)const {
		//loops through all convexes
		float minDistance = std::numeric_limits<float>::max();
		for (auto& conv : m_convexes) {
			//updates the min distance if the distance to this convex is lower
			minDistance = std::fminf(minDistance, conv->distanceFrom(localPoint));
		}
		//returns the lowest distance found
		return minDistance;
	}

	//destructor
	Concave::~Concave() {
		//deletes all the convexes
		for (auto& conv : m_convexes) {
			delete conv;
		}
		m_convexes.clear();
	
	}

}