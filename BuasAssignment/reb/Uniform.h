#ifndef REBOOTUNIFORM_H
#define REBOOTUNIFORM_H

#pragma once
#include <string>
#include <variant>
#include <SFML/Graphics.hpp>

namespace reb {

	//a value to be passed to a shader
	struct Uniform {

		//typedef for a variant of every possible uniform type
		typedef std::variant<float, int, sf::Glsl::Vec2, sf::Glsl::Vec3, sf::Glsl::Vec4, const sf::Texture*> UniformValue;

		//enum for the uniform types
		enum UniformType {
			UNIFORM_FLOAT, UNIFORM_INT, UNIFORM_VEC2, UNIFORM_VEC3, UNIFORM_VEC4, UNIFORM_SAMPLER2D
		};

		//the name of the uniform
		std::string m_name;

		//the value of the uniform
		UniformValue m_value;
		//the index of the variant type the value has
		UniformType m_valueType;

		//constructor
		Uniform(std::string name, UniformValue value)
			: m_name{ name }, m_value{ value }, m_valueType{ (UniformType)value.index() } {};

	};

}

#endif