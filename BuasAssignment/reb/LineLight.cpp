#include "LineLight.h"
#include "Container.h"
#include "Transform.h"

namespace reb {

	//no args-constructor
	LineLight::LineLight() : LightSource({ Uniform{"line", sf::Glsl::Vec4{0, 0, 0, 0}}, Uniform{"radius", 0.0f} }) {};

	//real constructor
	LineLight::LineLight(Color color, float intensity, float radius, std::pair<Vector2, Vector2> line)
		:	LightSource{ color, intensity }, 
			m_radius{ radius }, 
			m_line{ line }
	{
		Vector2 localLine = m_line.second - m_line.first;
		m_baseAABB = Rect<float>{ m_line.first.x - m_radius, m_line.first.y - m_radius, localLine.x + m_radius * 2, localLine.y + m_radius * 2 };
		setUniform(Uniform{ "radius", m_radius });
	}

	LineLight::LineLight(Color color, float intensity, float radius, Vector2 lineEnd)
		: LightSource{ color, intensity },
		m_radius{ radius },
		m_line{ std::make_pair(Vector2{}, lineEnd) }
	{
		Vector2 localLine = m_line.second - m_line.first;
		m_baseAABB = Rect<float>{ m_line.first.x - m_radius, m_line.first.y - m_radius, localLine.x + m_radius * 2, localLine.y + m_radius * 2 };
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
	std::string LineLight::getLightFunc(std::string lightName)const {
		return
"	vec2 A = line.xy;\n\
	vec2 B = line.ba;\n\
	float f = (B.y - A.y) / (B.x - A.x);\n\
	float x = (pos.y + (pos.x / f) - A.y + (f * A.x)) / (f + (1 / f));\n\
	float y = f * x + A.y - f * A.x;\n\
	vec3 perpPos = vec3(x, y, lightPos.z);\n\
	if(perpPos.x < min(A.x, B.x) || perpPos.x > max(A.x, B.x)){\n\
		if(length(pos.xy - A) < length(pos.xy - B)){\n\
			perpPos.xy = A;\n\
		}else{\n\
			perpPos.xy = B;\n\
		}\n\
	}\n\
	float distance = length(perpPos - pos);\n\
	vec3 direction = perpPos - pos;\n\
	direction = normalize(direction);\n\
	float att = pow(clamp(1.0 - pow(distance, 2) / pow(radius, 2), 0.0, 1.0), 2.0);\n\
	return mainLighting(diffuseColor, lightColor, lightIntensity * att, direction, normal, att);";
	}

	//returns a copy of the component
	LineLight* LineLight::clone()const {
		return new LineLight(*this);
	}

	//returns true if the lightsource has any effect within the given viewbox
	bool LineLight::onScreen(sf::FloatRect& viewBox)const {
		if (hasOwner()) {
			Vector2 lightPos = getWorldPos();
			return viewBox.intersects(sf::FloatRect{ lightPos.x + m_baseAABB.left, lightPos.y + m_baseAABB.top, m_baseAABB.width, m_baseAABB.height });
		}
	}

	//radius functions
	float LineLight::getRadius() {
		return m_radius;
	}

	void LineLight::setRadius(float newRadius) {
		m_radius = newRadius;
		setUniform(Uniform{ "radius", m_radius });
	}

	//line functions
	std::pair<Vector2, Vector2> LineLight::getLine() {
		return m_line;
	}

	void LineLight::setLine(std::pair<Vector2, Vector2> newLine) {
		m_line = newLine;
	}

	//updates the line in the uniforms to be in world space
	void LineLight::updateUniforms() {
		if (hasOwner()) {
			Vector2 a = getOwner()->getTransform()->getWorldSpace(m_line.first);
			Vector2 b = getOwner()->getTransform()->getWorldSpace(m_line.second);
			setUniform(Uniform("line", sf::Glsl::Vec4(a.x, a.y, b.x, b.y)));
		}
	}

}