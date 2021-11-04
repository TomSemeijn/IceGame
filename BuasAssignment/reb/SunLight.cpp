#include "SunLight.h"
#include <SFML/Graphics.hpp>

namespace reb {

	//no args-constructor
	SunLight::SunLight() : LightSource({ Uniform{"direction", sf::Glsl::Vec2{0, 0}} }) {  };

	//real constructor
	SunLight::SunLight(Color color, float Intensity, Vector2 direction) : LightSource{ color, Intensity }, m_dir{ direction * -1 }{
		setUniform(Uniform{ "direction", sf::Glsl::Vec2{m_dir.x, m_dir.y} });
	};

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
	std::string SunLight::getLightFunc(std::string lightName)const {
		return
			"	return mainLighting(diffuseColor, lightColor, lightIntensity, vec3(direction, lightPos.z - pos.z), normal, 1.0);";
	};

	//returns a copy of the component
	SunLight* SunLight::clone()const {
		return new SunLight{ *this };
	}

	//returns true if the lightsource has any effect within the given viewbox
	bool SunLight::onScreen(sf::FloatRect& viewBox)const {
		return true;
	}

}