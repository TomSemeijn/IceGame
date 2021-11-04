#include "LightSource.h"
#include "Transform.h"
#include "Container.h"

namespace reb {

	//real constructor
	LightSource::LightSource(Color color, float Intensity)
		:	m_lightColor{ color }, 
			m_intensity{ Intensity }
	{}

	//sets the color
	void LightSource::setColor(Color newColor) { 
		m_lightColor = newColor; 
	}

	//gets the color
	Color LightSource::getColor() { 
		return m_lightColor; 
	}

	//sets the intensity
	void LightSource::setIntensity(float newIntensity) { 
		m_intensity = newIntensity; 
	}

	//gets the intensity
	float LightSource::getIntensity() { 
		return m_intensity; 
	}

	//sets the offset
	void LightSource::setOffset(Vector3 newOffset) {
		m_offset = newOffset;
	}

	//sets the offset
	void LightSource::setOffset(Vector2 newOffset) {
		m_offset = Vector3(newOffset.x, newOffset.y, m_offset.z);
	}

	//gets the offset
	Vector3 LightSource::getOffset()const {
		return m_offset;
	}

	//returns the world position of this lightSource
	Vector2 LightSource::getWorldPos()const {
		return getOwner()->getTransform()->getWorldSpace(Vector2(m_offset.x, m_offset.y));
	}

	Vector3 LightSource::getFullWorldPos()const {
		return Vector3(getOwner()->getTransform()->getWorldSpace(Vector2(m_offset.x, m_offset.y)), m_offset.z + getOwner()->getTransform()->getZ());
	}

	//sets a uniform
	void LightSource::setUniform(Uniform uniform) {
		auto iter = std::find_if(m_uniforms.begin(), m_uniforms.end(), [&uniform](Uniform& element) {return element.m_name == uniform.m_name; });
		if (iter != m_uniforms.end()) {
			iter->m_value = uniform.m_value;
		}
		else {
			m_uniforms.push_back(uniform);
		}
	}

	//returns a const reference to the uniforms
	const std::vector<Uniform>& LightSource::getUniforms() {
		updateUniforms();
		return m_uniforms;
	}

	//removes the lightsource from its owning object
	void LightSource::destroy() {
		if (hasOwner()) {
			getOwner()->clearLight();
		}
	}

}