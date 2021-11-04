#ifndef REBOOTSHADER_H
#define REBOOTSHADER_H

#pragma once
#include <fstream>
#include <unordered_set>
#include <string>
#include <SFML/Graphics.hpp>
#include "LightSource.h"
#include "LightTypeInfo.h"
#include "Content.h"
#include "LightSourceInfo.h"
#include "Transform.h"
#include "DrawBatch.h"

namespace reb {

	class Scene;
	class ShadedSprite;

	//holds the shader to be used in shaded sprites, it constructs this shader with the lightsources registered to it
	class Shader {
	private:
		//a collection of uniforms used to order them into vectors so they can be set all at once
		struct UniformCollection {

			//a map for every type the uniform can be, indexed by their in-shader name
			std::map<std::string, std::vector<float>> m_floats;
			std::map<std::string, std::vector<int>> m_ints;
			std::map<std::string, std::vector<sf::Glsl::Vec2>> m_vec2s;
			std::map<std::string, std::vector<sf::Glsl::Vec3>> m_vec3s;
			std::map<std::string, std::vector<sf::Glsl::Vec4>> m_vec4s;
			std::map<std::string, std::vector<const sf::Texture*>> m_samples2Ds;

			//adds a uniform value to the collection
			void addUniform(std::string lightName, const Uniform& uniform);

			//sets all the uniforms to the referenced shader
			void setUniforms(sf::Shader& shader);
		};

		//the final sfml shader
		sf::Shader m_shader;

		//the vector of light types
		std::vector<LightTypeInfo> m_types;

		//returns if the given light type is within the vector
		bool hasType(std::string name);

		//keeps a pointer to the texture of the last rendered sprite to avoid unnecessary updates
		sf::Texture* m_lastTexture = nullptr;

		//keeps the last used values to avoid unnecessary updates
		int m_lastCells = 0;
		float m_lastAlpha = 0;
		Color m_lastColor = Color();
		float m_lastRotation = 0;
		float m_lastZ = 0;
		bool m_lastFlippedX = false;
		bool m_lastFlippedY = false;

		//returns a processed typename
		std::string getTypename(LightSource* dummy);

		template <typename LightType>
		std::string getTypename() {
			return getTypename(LightType{});
		};
		
		//constructs a unfiform string
		std::string getUniformStr(LightSource* dummy, int maxInstances, std::string lightName);

		//constructs a function string
		std::string getFunctionStr(LightSource* dummy, std::string lightName);

	public:
		//the color of the ambient light
		static Color s_ambientColor;
		//the intensity of the ambient light [0 => 1]
		static float s_ambientIntensity;

		//adds a lightType to the vector using a pointer
		void addLightType(LightSource* dummy, int maxInstances);

		//adds a lighttype to the vector using a template
		template <typename LightType>
		void addLightType(int maxInstances) {
			//makes sure only extensions of lightsource are used
			static_assert(std::is_base_of<LightSource, LightType>::value, "The LightType passed to Shader::addLightType is not an extension of Reboot::LightSource");
			//creates a shared pointer instance of the lightsource
			LightSource* dummy = new LightType();
			//adds lightType
			addLightType(dummy, maxInstances);
			//frees memory
			delete dummy;
		};

		//clears the vector of all lightTypes
		void clearLightTypes();

		//builds and sets the shader according to the current lightTypes
		void buildShader();

		//sets all the uniform values of the shader
		void setUniforms(Scene const * const scene, sf::RenderTarget& window, sf::FloatRect& viewBox);

		//draws a drawbatch
		void drawLayer(DrawLayer& layer, sf::RenderTarget& window);

	private:
		//====================== constant parts of the glsl shader =======================================

		//the start of the shader defining all the usual uniforms
		const std::string SHADERSTART =
"#version 120\n\
\n\
uniform vec3 ambientColor;\n\
uniform float ambientIntensity;\n\
\n\
uniform sampler2D spriteMap;\n\
uniform sampler2D normalMap;\n\
uniform sampler2D infoMap;\n\
\n\
uniform vec4 viewBox;\n\
uniform vec2 resolution;\n\
uniform vec4 viewport;\n\
\n\
const float PI = 3.14159;\n\
\n\
int alpha;\n\
bool useSpecular;\n\
int cells;\n\
float normalRotation;\n\
bool flipX;\n\
bool flipY;\n\
float z;\n\
\n\
vec2 getRotatedAround(vec2 point, vec2 center, float radians){\n\
	float sine = sin(radians);\n\
	float cosine = cos(radians);\n\
	float x = point.x - center.x;\n\
	float y = point.y - center.y;\n\
	return center + \n\
		vec2(\n\
			x * cosine - y * sine, \n\
			x * sine + y * cosine \n\
		);\n\
}\n\
\n\
vec2 getWorldNormal(vec2 localNormal){\n\
	if(flipX){ localNormal.x *= -1.0; }\n\
	if(flipY){ localNormal.y *= -1.0; }\n\
	return getRotatedAround(localNormal, vec2(0, 0), normalRotation);\n\
}\n\
\n\
vec3 mainSpecular(vec3 diffuseColor, vec3 lightColor, vec3 direction, vec3 normal, float attenuation){\n\
	vec3 reflected = reflect(-direction, normal);\n\
	return pow(clamp(dot(reflected, vec3(0.0, 0.0, 1.0)), 0.0, 1.0), alpha) * diffuseColor * 0.5 * lightColor * attenuation;\n\
}\n\
\n\
vec3 mainLighting(vec3 diffuseColor, vec3 lightColor, float intensity, vec3 direction, vec3 normal, float attenuation){\n\
	direction = normalize(direction);\n\
	float diffuse = max(dot(direction, normal), 0.0);\n\
	vec3 endColor = lightColor * diffuse;\n\
	endColor = vec3(endColor.x * diffuseColor.x, endColor.y * diffuseColor.y, endColor.z * diffuseColor.z);\n\
	endColor = (endColor / 255.0) * intensity;\n\
	endColor = vec3(min(endColor.x, 1.0), min(endColor.y, 1.0), min(endColor.z, 1.0));\n\
	if(useSpecular){ endColor += mainSpecular(diffuseColor, lightColor, direction, normal, attenuation); }\n\
	return endColor;\n\
}\n\
\n";

		//the start of the main function of the shader, finding the diffuse color normal and world position of the current pixel
		//and adding the ambient light to it
		const std::string SHADERMAINSTART =
"void main(){\n\
	vec4 diffuseColor = texture2D(spriteMap, gl_TexCoord[0].xy);\n\
	if (diffuseColor.a == 0){discard;}\n\
	\n\
	vec4 infoColor = texture2D(infoMap, gl_TexCoord[0].xy);\n\
	cells = int(floor(infoColor.a * 255.0));\n\
	if(cells == 0){ gl_FragColor = diffuseColor; return; }\n\
	\n\
	alpha = int(infoColor.r * 255.0);\n\
	flipX = (infoColor.g > 0.45 && infoColor.g < 0.55) || infoColor.g > 0.9;\n\
	flipY = (infoColor.g > 0.7 && infoColor.g < 0.8) || infoColor.g > 0.9;\n\
	useSpecular = alpha > 0;\n\
	normalRotation = infoColor.b * 2 * PI;\n\
	\n\
	vec3 resultColor = vec3(pow(ambientIntensity, 2) * ambientColor * diffuseColor.rgb);\n\
	\n\
	vec2 screenPos = vec2(gl_FragCoord.xy / resolution.xy);\n\
	screenPos = vec2(screenPos.x, 1.0 - screenPos.y);\n\
	screenPos -= viewport.xy;\n\
	screenPos /= viewport.ba;\n\
	\n\
	vec4 normalColor = texture2D(normalMap, gl_TexCoord[0].xy);\n\
	z = ((normalColor.a * 2.0) - 1.0) * 255.0;\n\
	vec3 worldPos = vec3(viewBox.xy + viewBox.ba * screenPos, z);\n\
	vec3 normal = (normalColor.rgb - 0.5) * 2.0;\n\
	normal = normalize(normal);\n\
	normal.y *= -1.0;\n\
	normal = vec3(getWorldNormal(normal.xy), normal.z);\n";

		//the end of the shader, setting the final color to the result of all lightsources with the alpha value
		//of the original diffuse color
		const std::string SHADEREND =
"	resultColor *= cells; resultColor = floor(resultColor); resultColor /= cells - 0.5;\n\
	gl_FragColor = vec4(resultColor * gl_Color.rgb, diffuseColor.a * gl_Color.a);\n\
	//gl_FragColor = diffuseColor;\n\
	//gl_FragColor = normalColor;\n\
	//gl_FragColor = vec4(screenPos, 0.0, 1.0);\n\
	//gl_FragColor = vec4(worldPos.x / 480.0, worldPos.y / 270.0, 0.0, 1.0);\n\
	//gl_FragColor = vec4(normalColor.rgb, 1.0);\n\
}\n";

	};

}

#endif