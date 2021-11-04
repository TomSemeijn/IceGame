#ifndef REBOOTLINELIGHT_H
#define REBOOTLINELIGHT_H

#pragma once
#include "LightSource.h"
#include "Rect.h"

namespace reb {

	//a lightsource defined by two local-space points that form a line segment which radiates light the same way a
	//pointlight would do
	class LineLight : public LightSource {
	public:
		//no args-constructor
		LineLight();

		//real constructor
		LineLight(Color color, float intensity, float radius, std::pair<Vector2, Vector2> line);
		LineLight(Color color, float intensity, float radius, Vector2 lineEnd);

	protected:
		Rect<float> m_baseAABB;
		float m_radius;
		std::pair<Vector2, Vector2> m_line;

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
		//you also have access to the function 
		//vec3 mainLighting(vec3 diffuseColor, vec3 lightColor, float intensity, vec3 direction, vec3 normal)
		//for easy standardized lighting
		virtual std::string getLightFunc(std::string lightName)const override;

		//returns a copy of the component
		virtual LineLight* clone()const override;

		//returns true if the lightsource has any effect within the given viewbox
		virtual bool onScreen(sf::FloatRect& viewBox)const override;

		//updates the line in the uniforms to be in world space
		virtual void updateUniforms()override;

	public:
		//radius functions
		float getRadius();
		void setRadius(float newRadius);

		//line functions
		std::pair<Vector2, Vector2> getLine();
		void setLine(std::pair<Vector2, Vector2> newLine);
	};

}

#endif