#ifndef REBOOTCOLOR_H
#define REBOOTCOLOR_H

#pragma once
#include <SFML/Graphics.hpp>
#include "Vector3.h"

namespace reb {

	//defines a simple rgba color, values are clamped to [0 => 255]
	struct Color {
	private:
		//the rgba components of the color
		int r, g, b, a;
	public:
		//default constructor setting everything to 0
		Color() : r{ 0 }, g{ 0 }, b{ 0 }, a{ 0 }{};
		//constructor taking integers for every value, making sure they're between 0 and 255 and giving the a channel
		//a default value of 255 because that's usually the case
		Color(int R, int G, int B, int A = 255)
			: r{ R < 0 ? 0 : (R > 255 ? 255 : R) },
			g{ G < 0 ? 0 : (G > 255 ? 255 : G) },
			b{ B < 0 ? 0 : (B > 255 ? 255 : B) },
			a{ A < 0 ? 0 : (A > 255 ? 255 : A) }
		{};
		//constructor taking an sfml color object
		Color(sf::Color sfmlColor) : r{ sfmlColor.r }, g{ sfmlColor.g }, b{ sfmlColor.b }, a{ sfmlColor.a }{};
		//constructor taking a Vector3 for rgb and a seperate integer for the a channel with a default of 255
		Color(Vector3 col, int A = 255) {
			setR((int)col.x);
			setG((int)col.y);
			setB((int)col.z);
			setA(A);
		}

		//sets any of the rgba channels while making sure they're between 0 and 255
		void setR(int R) { r = R < 0 ? 0 : (R > 255 ? 255 : R); };
		void setG(int G) { g = G < 0 ? 0 : (G > 255 ? 255 : G); };
		void setB(int B) { b = B < 0 ? 0 : (B > 255 ? 255 : B); };
		void setA(int A) { a = A < 0 ? 0 : (A > 255 ? 255 : A); };

		//gets any of the rgba channels
		int getR()const { return r; };
		int getG()const { return g; };
		int getB()const { return b; };
		int getA()const { return a; };

		//+ operator for adding colors together
		Color operator+(Color other)const {
			return Color(r + other.getR(), g + other.getG(), b + other.getB(), a);
		};
		//+= operator for adding colors together
		Color operator+=(Color other) {
			setR(r + other.getR());
			setG(g + other.getG());
			setB(b + other.getB());
			return *this;
		}

		//returns if the color is transparent or not
		bool transparent()const { return a == 0; };
		
		//returns a normalized directional Vector3 of the rgb channels
		Vector3 getDirectional()const { return Vector3(r, g, b) / 255; };
		//returns the rgb channels as a Vector3
		Vector3 getVec()const { return Vector3{ (float)r,(float) (float)g, (float)b }; };
		//returns an sfml color object containing this color
		sf::Color getSfml()const { return sf::Color(r, g, b, a); };
		//returns a glsl vec4 containing this normalized color
		sf::Glsl::Vec4 getGlsl()const { return sf::Glsl::Vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f); };

		//== operator
		bool operator==(const Color rhs) {
			return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
		};

		//!= operator
		bool operator!=(const Color rhs) {
			return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a;
		};

	};

}

#endif