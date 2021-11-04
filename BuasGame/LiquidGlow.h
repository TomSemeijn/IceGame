#pragma once
#include <reb/LightSource.h>

class LiquidGlow : public reb::LightSource {
private:
	float m_radius;
	sf::RenderTexture m_glowMap;
	sf::RenderTexture m_normalMap;
	sf::Shader m_mapShader;
	sf::Shader m_normalShader;
	int m_lastCenterAmount;

	mutable sf::FloatRect m_viewBox;

	//rebuilds the shader
	void rebuildShader(int dropAmount);
protected:
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
	std::string getLightFunc(std::string lightName)const override;

	//called before uniforms are returned to give the lightsource a chance to update things if necessary
	void updateUniforms()override;
public:
	//constructor
	LiquidGlow();
	LiquidGlow(reb::Color color, float Intensity, float radius);

	//returns true if the lightsource has any effect within the given viewbox
	bool onScreen(sf::FloatRect& viewBox)const override;

	//returns a clone
	LiquidGlow* clone()const override;

};