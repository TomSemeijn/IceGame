#ifndef REBOOTPOINTLIGHT_H
#define REBOOTPOINTLIGHT_H

#pragma once
#include "LightSource.h"
#include <SFML/Graphics.hpp>

namespace reb {

	//a lightsource defined by one point that radiates light in every direction up until a certain radius,
	//intensity is lost quadratically over the course of this radius
	class PointLight : public LightSource {
	public:
		//no args-constructor
		PointLight();

		//real constructor
		PointLight(Color color, float Intensity, float Radius);

	protected:
		float m_radius;

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
		virtual PointLight* clone()const override;

		//returns true if the lightsource has any effect within the given viewbox
		virtual bool onScreen(sf::FloatRect& viewBox)const override;

	public:
		//radius functions
		float getRadius();
		void setRadius(float newRadius);
	};

}

#endif