#ifndef REBOOTRECT_H
#define REBOOTRECT_H

#pragma once
#include <iostream>
#include "Vector2.h"
#include "Functions.h"

namespace reb {

	//a rectangle that can be of any numeric type
	template <typename T>
	struct Rect {
		//position and size
		T left;
		T top;
		T width;
		T height;

		//default constructor
		Rect() : left{ 0 }, top{ 0 }, width{ 0 }, height{ 0 }{};
		//main constructor
		Rect(T Left, T Top, T Width, T Height) : left{ Left }, top{ Top }, width{ Width }, height{ Height }{};

		//== operator
		bool operator==(const Rect<T>& other)const {
			return left == other.left && top == other.top && width == other.width && height == other.height;
		}

		//!= operator
		bool operator!=(const Rect<T>& other)const {
			return !(*this == other);
		}

		//returns the right position
		T getRight()const { return left + width; };
		//returns the bottom position
		T getBottom()const { return top + height; };

		//returns the area of the rectangle
		T getArea()const { return width * height; };

		//+ operator for adding vectors to change the position of the rectangle
		Rect<T> operator+(const Vector2 rhs)const { return Rect<T>(left + rhs.x, top + rhs.y, width, height); };
		//- operator for subtracting vectors to change the position of the rectangle
		Rect<T> operator-(const Vector2 rhs)const { return Rect<T>(left - rhs.x, top - rhs.y, width, height); };

		//returns true if the given point is within the rectangle
		bool withinRect(Vector2 p)const {
			return p.x >= left && p.x <= (left + width) && p.y >= top && p.y <= (top + height);
		};

		//returns true if the given point is within the rectangle
		bool withinRect(T& x, T& y)const {
			return x >= left && x <= getRight() && y >= top && y <= getBottom();
		};

		//returns true if the given line intersects with the rectangle
		bool intersectsLine(Vector2 a, Vector2 b)const {

			//returns true if either vertex of the line is within the rectangle
			if (withinRect(a) || withinRect(b)) { return true; }

			//returns true if the line intersects with any of the lines of the rectangle
			if (linesIntersect(a, b, Vector2(left, top), Vector2(left + width, top))) { return true; }
			if (linesIntersect(a, b, Vector2(left + width, top), Vector2(left + width, top + height))) { return true; }
			if (linesIntersect(a, b, Vector2(left + width, top + height), Vector2(left, top + height))) { return true; }
			if (linesIntersect(a, b, Vector2(left, top + height), Vector2(left, top))) { return true; }
			
			//returns false because all tests failed
			return false;
		};

		//checks for intersection with other rectangle
		bool intersectsRectAt(Rect<T>& other, Rect<T>& intersection)const {
			float myright = getRight();
			if (other.left > myright) { return false; }
			float otherRight = other.getRight();
			//checks if there is an intersection and returns false is not
			if (left > otherRight) { return false; }
			float mybottom = getBottom();
			if (other.top > mybottom) { return false; }
			float otherBottom = other.getBottom();
			if (top > otherBottom) { return false; }
			//gets the intersection area of the aabb and sets it to the referenced intersection aabb
			T intleft = (left > other.left ? left : other.left);
			T inttop = (top > other.top ? top : other.top);
			T intright = (myright < otherRight ? myright : otherRight);
			T intbottom = (mybottom < otherBottom ? mybottom : otherBottom);

			intersection = Rect<T>(intleft, inttop, intright - intleft, intbottom - inttop);
			//returns true
			return true;
		}

		//checks for intersection with other rectangle
		bool intersectsRect(Rect<T>& other)const {
			float myright = getRight();
			if (other.left > myright) { return false; }
			float otherRight = other.getRight();
			//checks if there is an intersection and returns false is not
			if (left > otherRight) { return false; }
			float mybottom = getBottom();
			if (other.top > mybottom) { return false; }
			float otherBottom = other.getBottom();
			if (top > otherBottom) { return false; }

			//returns true
			return true;
		}

		//returns the center of the rectangle as a vector2
		Vector2 getCenter()const {
			return Vector2(left + width * 0.5f, top + height * 0.5f);
		}

	};

	//defines the << operator for easy printing
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const Rect<T> rect) {
		os
			<< "{Left = "
			<< rect.left
			<< ", Top = "
			<< rect.top
			<< ", Width = "
			<< rect.width
			<< ", Height = "
			<< rect.height
			<< "}";
		return os;
	}
}

#endif