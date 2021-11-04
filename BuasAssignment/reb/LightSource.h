#ifndef REBOOTLIGHTSOURCE_H
#define REBOOTLIGHTSOURCE_H

#pragma once
#include "MainComponent.h"
#include <string>
#include "Vector3.h"
#include "Color.h"
#include "Uniform.h"

namespace reb {

	class Shader;
	class Component;
	class Container;

	//an abstract class that defines the behavior of a lightsource with a glsl function in string format
	class LightSource : public MainComponent {

		//defines shader as a friend so only the shader can access the functions from the outside
		friend Shader;

		//defines container as a friend so it can set itself as owner
		friend Container;

	private:

		//the offset of the lightsource
		Vector3 m_offset;

		//all the uniforms of the lightsource
		std::vector<Uniform> m_uniforms;

	protected:

		Color m_lightColor;
		float m_intensity;

		//returns the function body of the glsl function that adds the result of this lightSource
		//this function takes the following paramters:
		//			vec3 pos = the world position of the current pixel
		//			vec3 normal = the directional vector of the normal of the current pixel
		//			vec3 diffuseColor = the color on the diffuse map of the current pixel
		//			int lightIndex = the index of the lightSources type
		//			vec3 lightPos = the world position of the lightSource
		//			vec3 lightColor = the color of the lightSource
		//			float lightIntensity = the intensity of the lightSource
		//the function returns a vec3 of the color to add to the current pixel
		//
		//you also have access to the function:
		//vec3 mainDiffuse(vec3 diffuseColor, vec3 lightColor, float intensity, vec3 direction, vec3 normal)
		//for easy standardized lighting
		virtual std::string getLightFunc(std::string lightName)const = 0;

		//sets a uniform
		void setUniform(Uniform uniform);

		//called before uniforms are returned to give the lightsource a chance to update things if necessary
		virtual void updateUniforms() {};

	public:

		//constructor with default uniform values
		LightSource(std::vector<Uniform> uniforms) : m_uniforms{ uniforms } {};

		//real constructor
		LightSource(Color color, float Intensity);

		//sets the color
		void setColor(Color newColor);
		//gets the color
		Color getColor();
		//sets the intensity
		void setIntensity(float newIntensity);
		//gets the intensity
		float getIntensity();
		//sets the offset
		void setOffset(Vector3 newOffset);
		void setOffset(Vector2 newOffset);
		//gets the offset
		Vector3 getOffset()const;

		//returns the world position of this lightSource
		Vector2 getWorldPos()const;
		Vector3 getFullWorldPos()const;

		//returns true if the lightsource has any effect within the given viewbox
		virtual bool onScreen(sf::FloatRect& viewBox)const = 0;

		//returns a const reference to the uniforms
		const std::vector<Uniform>& getUniforms();

		//removes the lightsource from its owning object
		void destroy()override;

		//virtual destructor
		virtual ~LightSource() {};

	};

};

#endif