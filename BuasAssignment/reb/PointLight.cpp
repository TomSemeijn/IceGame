#include "PointLight.h"

namespace reb {

	//no args-constructor
	PointLight::PointLight() : LightSource{ {Uniform{"radius", 0.0f}} } {};

	//real constructor
	PointLight::PointLight(Color color, float Intensity, float Radius) : LightSource{ color, Intensity }, m_radius{Radius} {
		setUniform(Uniform{ "radius", m_radius });
	}

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
	std::string PointLight::getLightFunc(std::string lightName)const {
		return
"	float distance = length(lightPos - pos);\n\
	vec3 direction = lightPos - pos;\n\
	direction = normalize(direction);\n\
	float att = pow(clamp(1.0 - pow(distance, 2.0) / pow(radius, 2.0), 0.0, 1.0), 2.0);\n\
	return mainLighting(diffuseColor, lightColor, lightIntensity * att, direction, normal, att);";
	};

	//returns a copy of the component
	PointLight* PointLight::clone()const {
		return new PointLight{ *this };
	}

	//returns true if the lightsource has any effect within the given viewbox
	bool PointLight::onScreen(sf::FloatRect& viewBox)const {
		//gets the world position of the lightsource
		Vector2 pos = getWorldPos();
		//closest distance from pos to aabb (real-time collisionm detection page 131 - 132)
		float sqDist = 0;
		//x axis
		if (pos.x < viewBox.left) {
			sqDist += (viewBox.left - pos.x) * (viewBox.left - pos.x);
		}
		float boxRight = viewBox.left + viewBox.width;
		if (pos.x > boxRight) {
			sqDist += (pos.x - boxRight) * (pos.x - boxRight);
		}
		//y axis
		if (pos.y < viewBox.top) {
			sqDist += (viewBox.top - pos.y) * (viewBox.top - pos.y);
		}
		float boxBottom = viewBox.top + viewBox.height;
		if (pos.y > boxBottom) {
			sqDist += (pos.y - boxBottom) * (pos.y - boxBottom);
		}
		//returns true if the closest distance to the aabb is less than the radius
		return sqDist < (m_radius * m_radius);
	}

	//line functions
	float PointLight::getRadius() {
		return m_radius;
	}

	void PointLight::setRadius(float newRadius) {
		m_radius = newRadius;
		setUniform(Uniform{ "radius", m_radius });
	}

}