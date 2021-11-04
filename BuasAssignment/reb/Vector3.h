#ifndef REBOOTVECTOR3_H
#define REBOOTVECTOR3_H

#pragma once
#include <cmath>
#include <iostream>
#include <SFML/System.hpp>
#include "Vector2.h"

namespace reb {

	//a three dimensional vector
	struct Vector3 {
		//the components of the vector
		float x;
		float y;
		float z;

		//default constructor with null values
		Vector3() : x{ 0 }, y{ 0 }, z{ 0 }{};
		//constructor with given values
		Vector3(float X, float Y, float Z) : x{ X }, y{ Y }, z{ Z }{};
		Vector3(Vector2 base, float Z) : x{ base.x }, y{ base.y }, z{ Z }{};
		//subscript operator overload for easier access
		float& operator[](const unsigned int index) {
			if (index == 0) { return x; }
			else if (index == 1) { return y; }
			else if (index == 2) { return z; }
			else { throw std::out_of_range("An index passed to the [] operator in a reb::Vector3 object was out of range."); }
		};

		// << operator for easy console writing
		std::ostream& operator<<(std::ostream& os) {
			os << "{X = " << x << ", Y = " << y << ", Z = " << z << "}";
			return os;
		}

		//the + operator
		Vector3 operator+(const Vector3 toAdd) const {
			return Vector3(x + toAdd.x, y + toAdd.y, z + toAdd.z);
		}
		//the += operator
		Vector3 operator+=(const Vector3 toAdd) {
			x += toAdd.x;
			y += toAdd.y;
			z += toAdd.z;
			return *this;
		}

		//the - operator
		Vector3 operator-(const Vector3 toSubtr) const {
			return Vector3(x - toSubtr.x, y - toSubtr.y, z - toSubtr.z);
		}
		//the -= operator
		Vector3 operator-=(const Vector3 toSubtr) {
			x -= toSubtr.x;
			y -= toSubtr.y;
			z -= toSubtr.z;
			return *this;
		}

		//the * operator using a float as a scalar
		Vector3 operator*(const float scalar) const {
			return Vector3(x * scalar, y * scalar, z * scalar);
		};
		//the *= operator using a float as a scalar
		Vector3 operator*=(const float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		};

		//the / operator using a float to generate a scalar
		Vector3 operator/(const float scalar) const {
			return *this * (1.0 / scalar);
		}
		//the /= operator
		Vector3 operator/=(const float scalar) {
			*this = *this / scalar;
			return *this;
		}

		//returns the dot product of the given vector and this vector
		float dotProduct(const Vector3 other) const {
			return x * other.x + y * other.y + z * other.z;
		};
		//returns the cross product of the given vector and this vector
		Vector3 crossProduct(const Vector3 other) const {
			return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
		};
		//returns the component-wise product of the given vector and this vector
		Vector3 compProduct(const Vector3 other) const {
			return Vector3(x * other.x, y * other.y, z * other.z);
		};

		//returns the current length of the vector
		float getLength() {
			return std::sqrtf(std::powf(std::sqrtf(std::powf(x, 2) + std::powf(y, 2)), 2) + std::powf(z, 2));
		};
		//keeps the vector pointing in the same direction but changes its length to the given
		void setLength(float newLength) {
			//gets the current length of the vector, divides the wanted length with that length and uses that as
			//a scalar to scale the vector to the new length
			*this *= std::fabsf(newLength / getLength());
		};

		//returns an sfml version of the vector
		sf::Vector3f getSfml() { return sf::Vector3f(x, y, z); };
	};

	//defines the << operator for easy printing
	inline std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
		os << "{X = " << vec.x << ", Y = " << vec.y << ", Z = " << vec.z << "}";
		return os;
	}

}

#endif