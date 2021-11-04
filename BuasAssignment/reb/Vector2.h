#ifndef REBOOTVECTOR2_H
#define REBOOTVECTOR2_H

#pragma once
#include <cmath>
#include <iostream>
#include <SFML/System.hpp>

namespace reb {

	//a two-dimensional vector
	struct Vector2 {
		//the components of the vector
		float x;
		float y;

		//default constructor with null values
		Vector2() : x{ 0 }, y{ 0 }{};
		//constructor with given values
		Vector2(float X, float Y) : x{ X }, y{ Y }{};
		Vector2(sf::Vector2f sf) : x{ sf.x }, y{ sf.y }{};
		Vector2(sf::Vector2i sf) : x{ (float)sf.x }, y{ (float)sf.y }{};

		//subscript operator overload for easier access
		float& operator[](const unsigned int index) {
			if (index == 0) { return x; }
			else if (index == 1) { return y; }
			else { throw std::out_of_range("An index passed to the [] operator in a reb::Vector2 object was out of range."); }
		};

		//the + operator
		Vector2 operator+(const Vector2& toAdd) const {
			return Vector2(x + toAdd.x, y + toAdd.y);
		}
		//the += operator
		Vector2 operator+=(const Vector2& toAdd) {
			x += toAdd.x;
			y += toAdd.y;
			return *this;
		}

		//the - operator
		Vector2 operator-(const Vector2& toSubtr) const {
			return Vector2(x - toSubtr.x, y - toSubtr.y);
		}
		//the -= operator
		Vector2 operator-=(const Vector2& toSubtr) {
			x -= toSubtr.x;
			y -= toSubtr.y;
			return *this;
		}

		//the * operator using a float as a scalar
		Vector2 operator*(const float scalar) const {
			return Vector2(x * scalar, y * scalar);
		};
		//the *= operator using a float as a scalar
		Vector2 operator*=(const float scalar) {
			x *= scalar;
			y *= scalar;
			return *this;
		};

		//the / operator using a float to generate a scalar
		Vector2 operator/(const float scalar) const {
			return *this * (1.0 / scalar);
		}
		//the /= operator
		Vector2 operator/=(const float scalar) {
			*this = *this / scalar;
			return *this;
		}

		//returns the dot product of the given vector and this vector
		float dotProduct(const Vector2& other) const {
			return x * other.x + y * other.y;
		};
		//the cross product of the given vector and this vector
		float crossProd(const Vector2& other)const {
			return x * other.y - y * other.x;
		}
		//the cross product with a scalar on the left
		Vector2 leftScalarCrossProd(const float scalar)const {
			return Vector2(-scalar * y, scalar * x);
		}
		//the cross product with a scalar on the right
		Vector2 rightScalarCrossProd(const float scalar)const {
			return Vector2(scalar * y, -scalar * x);
		}
		//returns the component-wise product of the given vector and this vector
		Vector2 compProduct(const Vector2 other) const {
			return Vector2(x * other.x, y * other.y);
		};

		//returns the component-wise division of the given vector and this vector
		Vector2 compDiv(const Vector2 other) const {
			return Vector2(x / other.x, y / other.y);
		}

		//returns a rounded version of the vector
		Vector2 getRounded()const {
			return Vector2(std::roundf(x), std::roundf(y));
		}
		//returns a floored version of the vector
		Vector2 getFloored()const {
			return Vector2(std::floorf(x), std::floorf(y));
		}
		//returns a ceiled version of the vector
		Vector2 getCeiled()const {
			return Vector2(std::ceilf(x), std::ceilf(y));
		}
		//returns the current length of the vector
		float getLength() {
			return std::sqrtf(std::powf(x, 2) + std::powf(y, 2));
		};
		//keeps the vector pointing in the same direction but changes its length to the given
		Vector2 setLength(float newLength) {
			//gets the current length of the vector, divides the wanted length with that length and uses that as
			//a scalar to scale the vector to the new length
			float currentLength = getLength();
			if (currentLength > 0) {
				*this *= newLength / currentLength;
			}
			return *this;
		};

		//returns an sfml version of the vector
		sf::Vector2f getSfml()const { return sf::Vector2f(x, y); };
	};
	//defines the < operator, 
	//interpreting the vector as a point on a 2D plane made of rows from left to right and top to bottom
	inline bool operator<(Vector2 lhs, Vector2 rhs) {
		return lhs.y != rhs.y ? lhs.y < rhs.y : lhs.x < rhs.x;
	}

	//defines the != operator
	inline bool operator!=(Vector2 lhs, Vector2 rhs) {
		return lhs.x != rhs.x || lhs.y != rhs.y;
	}

	//defines the == operator
	inline bool operator==(Vector2 lhs, Vector2 rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	//defines the << operator for easy printing
	inline std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
		os << "{X = " << vec.x << ", Y = " << vec.y << "}";
		return os;
	}
}

#endif