#ifndef REBOOTFUNCTIONS_H
#define REBOOTFUNCTIONS_H

#pragma once
#include <vector>
#include <cctype>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <random>
#include <filesystem>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "Vector2.h"

//some general functions that should be global
namespace reb {

	const float PI = 3.14159;
	const float EPSILON = 0.000001;

	//writes the given message to the console
	template <typename T>
	static void write(T msg)
	{
		std::cout << msg;
	}

	//writes the given message as a line to the console
	template <typename... messages>
	static void writeLn(messages... msg) {
		std::initializer_list<int>{(reb::write(msg), 0)...};
		std::cout << '\n';
	}

	//erase all Occurrences of given substring from main string. from "https://thispointer.com/how-to-remove-substrings-from-a-string-in-c/"
	std::string eraseAllSubStr(std::string & mainStr, const std::string & toErase);

	//translates the given point by a rotation of the given amount of radians around the given origin
	void translateRotation(Vector2& point, const Vector2& origin, const float radians);

	//returns a boolean for if the given lines intersect (https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect)
	bool linesIntersect(Vector2 a, Vector2 b, Vector2 x, Vector2 y);

	//returns a boolean for if the given lines intersect and sets the intersection point to the referenced vector (https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect)
	bool linesIntersectAt(Vector2 a, Vector2 b, Vector2 x, Vector2 y, Vector2& intersection);

	// trim from start (in place) (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605)
	void ltrim(std::string &s);

	// trim from end (in place) (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605)
	void rtrim(std::string &s);

	// trim from both ends (in place) (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605)
	void trim(std::string &s);

	//returns the amount of occurrences the given substring has in the given string (https://stackoverflow.com/questions/22489073/counting-the-number-of-occurrences-of-a-string-within-a-string)
	int countSubstr(std::string str, std::string sub);

	//splits the given string with the given delimiter (https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/)
	std::vector<std::string> split(const std::string& s, char delimiter);

	//replaces all occurrences the given substring in the referenced string (https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c)
	void replaceSubstr(std::string& str, std::string substr, std::string replacement);

	//converts degrees to radians
	float getRadians(float degrees);

	//converts radians to degrees
	float getDegrees(float radians);

	//converts a directional vector to an angle value (https://stackoverflow.com/questions/6247153/angle-from-2d-unit-vector)
	float directionToAngle(Vector2 dir);

	//converts an angle value to a directional vector
	Vector2 angleToDirection(float angle);

	//snaps to window pixel space
	void windowSnap(sf::RenderTarget* window, Vector2& vec);

	//removes the element at the given index using the swap and pop method
	template <typename T>
	typename std::vector<T>::iterator swapPop(std::vector<T>& vec, const unsigned int index) {
		if (vec.empty()) { return vec.end(); }
		//verifies the given index is within bounds
		if (vec.size() > index) {
			//verifies there's more than one element within the vector
			if (vec.size() >= 2) {
				//swaps with the last element of the vector if the given index isnt the last element already
				if (vec.begin() + index != vec.end() - 1) {
					std::iter_swap(vec.begin() + index, vec.end() - 1);
				}
				//pops the last element of the vector
				vec.pop_back();
			}//clears the vector there was only one element left
			else {
				vec.clear();
			}
		}
		return vec.begin() + index;
	}

	//finds and removes the given element using the swap and pop method if the element is found
	template <typename T>
	void swapPopEl(std::vector<T>& vec, const T& element) {
		if (vec.empty()) { return; }
		//finds the element
		auto place = std::find(vec.begin(), vec.end(), element);
		//verifies the element was found
		if (place != vec.end()) {
			//removes using swap and pop
			swapPop(vec, std::distance(vec.begin(), place));
		}
	}

	//finds and removes the given element using the swap and pop method with the given iterator
	template <typename T>
	void swapPopIter(std::vector<T>& vec, typename std::vector<T>::iterator place) {
		if (vec.empty()) { return; }
		//verifies the element was found
		if (place != vec.end()) {
			//removes using swap and pop
			swapPop(vec, std::distance(vec.begin(), place));
		}
	}

	//returns true if the requested element is within the vector
	template <typename T>
	bool withinVector(std::vector<T>& vec, const T& element) {
		return std::find(vec.begin(), vec.end(), element) != vec.end();
	}

	//prints a vector's contents to the ostream
	template <typename T>
	void printVec(const std::vector<T>& vec) {
		std::cout << "{";
		for (auto k = vec.begin(); k < vec.end(); k++) {
			if (k != vec.begin()) {
				std::cout << ", ";
			}
			std::cout << *k;
		}
		std::cout << "}" << std::endl;
	}

	//generates a random float
	float genRandf(float min, float max);

	//generates a random integer
	int genRandi(int min, int max);

	//turns a relative path string into a global path string
	std::string getGlobalPath(std::string relativePath, std::filesystem::path relativeTo);

	//returns the distance from a point to a line segment
	float distancePointLine(Vector2 a, Vector2 b, Vector2 p);

}

#endif