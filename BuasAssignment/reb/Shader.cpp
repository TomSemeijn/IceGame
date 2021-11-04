#include "Shader.h"
#include "GameObj.h"
#include "Functions.h"
#include "Scene.h"
#include "Tile.h"
#include "ShadedSprite.h"

namespace reb {

	//sets the default ambient values
	Color Shader::s_ambientColor = Color(255, 255, 255);
	float Shader::s_ambientIntensity = 0.6f;

	//returns if the given light type is within the vector
	bool Shader::hasType(std::string name) {
		return std::find_if(m_types.begin(), m_types.end(), [&name](const LightTypeInfo& a) {
			return a.name == name;
		}) != m_types.end();
	}

	//returns a processed typename
	std::string Shader::getTypename(LightSource* dummy) {
		std::string typeName = typeid(*dummy).name();
		eraseAllSubStr(typeName, "class");
		eraseAllSubStr(typeName, "::");
		eraseAllSubStr(typeName, "<");
		eraseAllSubStr(typeName, ">");
		replaceSubstr(typeName, "0", "zero");
		replaceSubstr(typeName, "1", "one");
		replaceSubstr(typeName, "2", "two");
		replaceSubstr(typeName, "3", "three");
		replaceSubstr(typeName, "4", "four");
		replaceSubstr(typeName, "5", "five");
		replaceSubstr(typeName, "6", "six");
		replaceSubstr(typeName, "7", "seven");
		replaceSubstr(typeName, "8", "eight");
		replaceSubstr(typeName, "9", "nine");
		trim(typeName);
		return typeName;
	}

	//constructs a unfiform string
	std::string Shader::getUniformStr(LightSource* dummy, int maxInstances, std::string lightName) {
		//gets the uniforms of the default lightsource
		auto& uniforms = dummy->getUniforms();

		//starts the string with a struct opening
		std::string str = "";

		//iterates through all the uniforms
		for (auto& form : uniforms) {

			str += "uniform ";

			//adds the type declaration
			switch (form.m_valueType) {
			case Uniform::UNIFORM_FLOAT:
				str += "float";
				break;
			case Uniform::UNIFORM_INT:
				str += "int";
				break;
			case Uniform::UNIFORM_SAMPLER2D:
				str += "sampler2D";
				break;
			case Uniform::UNIFORM_VEC2:
				str += "vec2";
				break;
			case Uniform::UNIFORM_VEC3:
				str += "vec3";
				break;
			case Uniform::UNIFORM_VEC4:
				str += "vec4";
				break;
			}

			//adds the name, array declaration and semicolon
			str += " " + lightName + form.m_name + "[" + std::to_string(maxInstances) + "];\n";
		}

		//returns the string
		return str;
	}

	//constructs a function string
	std::string Shader::getFunctionStr(LightSource* dummy, std::string lightName) {
		
		//opens the function
		std::string str = "\nvec3 " + lightName + "LightFunction(vec3 pos, vec3 normal, vec3 diffuseColor, int lightIndex){\n";
		
		//makes basic information available
		str += "	vec3 lightPos = " + lightName + "Pos[lightIndex];\n";
		str += "	vec3 lightColor = " + lightName + "Color[lightIndex] / 255.0;\n";
		str += "	float lightIntensity = " + lightName + "Intensity[lightIndex];\n";

		//holds the function body
		std::string funcBody = dummy->getLightFunc(lightName);

		//makes custom uniforms available
		auto& uniforms = dummy->getUniforms();
		for (auto& form : uniforms) {
			bool skipVar = false;
			str += "	";
			//adds the type declaration
			switch (form.m_valueType) {
			case Uniform::UNIFORM_FLOAT:
				str += "float";
				break;
			case Uniform::UNIFORM_INT:
				str += "int";
				break;
			case Uniform::UNIFORM_SAMPLER2D:
				replaceSubstr(funcBody, form.m_name, lightName + form.m_name + "[lightIndex]");
				skipVar = true;
				break;
			case Uniform::UNIFORM_VEC2:
				str += "vec2";
				break;
			case Uniform::UNIFORM_VEC3:
				str += "vec3";
				break;
			case Uniform::UNIFORM_VEC4:
				str += "vec4";
				break;
			}
			//adds the name and initialization
			if (!skipVar) {
				str += " " + form.m_name + " = " + lightName + form.m_name + "[lightIndex];\n";
			}
			else {
				str += "\n";
			}
		}

		//adds the function body
		str += funcBody;
		//closes the function
		str += "\n}\n";
		
		//returns the string
		return str;
	}

	//adds a lightType to the vector using a shared pointer
	void Shader::addLightType(LightSource* dummy, int maxInstances) {
		//gets the typename
		std::string typeName = getTypename(dummy);
		//checks if the lightType exists already
		if (!hasType(typeName)) {
			//creates a new lightInfo object
			LightTypeInfo newInfo{};
			//sets all the values of the lightInfo
			newInfo.name = typeName;
			newInfo.maxInstances = maxInstances;
			newInfo.lightFunc = getFunctionStr(dummy, typeName);
			newInfo.extraUniforms = getUniformStr(dummy, maxInstances, typeName);
			//adds the lightInfo to the vector
			m_types.push_back(newInfo);
		}
	}

	//clears the vector of all lightTypes
	void Shader::clearLightTypes() {
		m_types.clear();
	}

	//builds and sets the shader according to the current lightTypes
	void Shader::buildShader() {
		//initializes with start of any shader
		std::string finalShader = SHADERSTART;
		//adds uniforms and functions for lightsources of every type
		for (auto& light : m_types) {
			//adds position array uniform
			finalShader += "uniform vec3 " + light.name + "Pos[" + std::to_string(light.maxInstances) + "];\n";
			finalShader += "uniform vec3 " + light.name + "Color[" + std::to_string(light.maxInstances) + "];\n";
			finalShader += "uniform float " + light.name + "Intensity[" + std::to_string(light.maxInstances) + "];\n";
			//adds lightsource amount uniform
			finalShader += "uniform int " + light.name + "Amount;\n";
			//adds extra uniforms
			finalShader += light.extraUniforms;
			//adds the function the lightType uses
			finalShader += light.lightFunc;
		}
		//adds the start of the main function
		finalShader += SHADERMAINSTART;
		//adds calls to every light source
		for (auto& light : m_types) {
			//adds loop that adds all the lightsources of the current lighttype
			finalShader +=
				"	if(" + light.name + "Amount > 0){\n\
	for(int k = 0; k < " + light.name + "Amount; k++){\n\
			resultColor += " + light.name + "LightFunction(worldPos, normal, diffuseColor.rgb, k);\n\
		}\n\
	}\n";
		}
		//adds end of shader
		finalShader += SHADEREND;

		//saves the shader to a file for debugging
		std::ofstream out("Content/last_shader.frag");
		std::streambuf *coutbuf = std::cout.rdbuf();
		std::cout.rdbuf(out.rdbuf());
		std::cout << finalShader;
		std::cout.rdbuf(coutbuf);

		//sets initial uniform values
		m_shader.setUniform("cells", 0);
		m_shader.setUniform("alpha", 0);
		m_shader.setUniform("rotation", 0);
		m_shader.setUniform("flippedX", false);
		m_shader.setUniform("flippedY", false);
		m_shader.setUniform("z", 0);
		m_shader.setUniform("color", sf::Glsl::Vec4(
			(float)m_lastColor.getR() / 255.0f,
			(float)m_lastColor.getG() / 255.0f,
			(float)m_lastColor.getB() / 255.0f,
			(float)m_lastColor.getA() / 255.0f)
		);

		//sets the sfml shader to the result
		m_shader.loadFromMemory(finalShader, sf::Shader::Type::Fragment);

	}

	//adds a uniform value to the collection
	void Shader::UniformCollection::addUniform(std::string lightName, const Uniform& uniform) {
		
		//gets the in-shader name of the uniform
		std::string inShaderName = lightName + uniform.m_name;

		//distinguishes by type
		switch (uniform.m_valueType) {
		
		//if the uniform is a float
		case Uniform::UNIFORM_FLOAT:
			{
				//tries to find the vector
				auto iter = m_floats.find(inShaderName);
				//adds the vector if it isnt there yet
				if (iter == m_floats.end()) {
					m_floats.emplace(std::make_pair(inShaderName, std::vector<float>{}));
					iter = m_floats.find(inShaderName);
				}
				//adds the value to the vector
				iter->second.push_back(std::get<float>(uniform.m_value));
			}
			break;

		//if the uniform is an integer
		case Uniform::UNIFORM_INT:
			{
				//tries to find the vector
				auto iter = m_ints.find(inShaderName);
				//adds the vector if it isnt there yet
				if (iter == m_ints.end()) {
					m_ints.emplace(std::make_pair(inShaderName, std::vector<int>{}));
					iter = m_ints.find(inShaderName);
				}
				//adds the value to the vector
				iter->second.push_back(std::get<int>(uniform.m_value));
			}
			break;

		//if the uniform is a sampler2D
		case Uniform::UNIFORM_SAMPLER2D:
			{
				//tries to find the vector
				auto iter = m_samples2Ds.find(inShaderName);
				//adds the vector if it isnt there yet
				if (iter == m_samples2Ds.end()) {
					m_samples2Ds.emplace(std::make_pair(inShaderName, std::vector<const sf::Texture*>{}));
					iter = m_samples2Ds.find(inShaderName);
				}
				//adds the value to the vector
				iter->second.push_back(std::get<const sf::Texture*>(uniform.m_value));
			}
			break;

		//if the uniform is a vec2
		case Uniform::UNIFORM_VEC2:
			{
				//tries to find the vector
				auto iter = m_vec2s.find(inShaderName);
				//adds the vector if it isnt there yet
				if (iter == m_vec2s.end()) {
					m_vec2s.emplace(std::make_pair(inShaderName, std::vector<sf::Glsl::Vec2>{}));
					iter = m_vec2s.find(inShaderName);
				}
				//adds the value to the vector
				iter->second.push_back(std::get<sf::Glsl::Vec2>(uniform.m_value));
			}
			break;

		//if the uniform is a vec3
		case Uniform::UNIFORM_VEC3:
			{
				//tries to find the vector
				auto iter = m_vec3s.find(inShaderName);
				//adds the vector if it isnt there yet
				if (iter == m_vec3s.end()) {
					m_vec3s.emplace(std::make_pair(inShaderName, std::vector<sf::Glsl::Vec3>{}));
					iter = m_vec3s.find(inShaderName);
				}
				//adds the value to the vector
				iter->second.push_back(std::get<sf::Glsl::Vec3>(uniform.m_value));
			}
			break;

		//if the uniform is a vec4
		case Uniform::UNIFORM_VEC4:
			{
				//tries to find the vector
				auto iter = m_vec4s.find(inShaderName);
				//adds the vector if it isnt there yet
				if (iter == m_vec4s.end()) {
					m_vec4s.emplace(std::make_pair(inShaderName, std::vector<sf::Glsl::Vec4>{}));
					iter = m_vec4s.find(inShaderName);
				}
				//adds the value to the vector
				iter->second.push_back(std::get<sf::Glsl::Vec4>(uniform.m_value));
			}
			break;
		
		}
	}

	//sets all the uniforms to the referenced shader
	void Shader::UniformCollection::setUniforms(sf::Shader& shader) {
		//sets all the floats
		for (auto& uniform : m_floats) {
			shader.setUniformArray(uniform.first, uniform.second.data(), uniform.second.size());
		}
		//sets all the vec2s
		for (auto& uniform : m_vec2s) {
			shader.setUniformArray(uniform.first, uniform.second.data(), uniform.second.size());
		}
		//sets all the vec3s
		for (auto& uniform : m_vec3s) {
			shader.setUniformArray(uniform.first, uniform.second.data(), uniform.second.size());
		}
		//sets all the vec4s
		for (auto& uniform : m_vec4s) {
			shader.setUniformArray(uniform.first, uniform.second.data(), uniform.second.size());
		}
		//sets all the integers
		for (auto& uniform : m_ints) {
			for (int k = 0; k < uniform.second.size(); k++) {
				shader.setUniform(uniform.first + "[" + std::to_string(k) + "]", uniform.second[k]);
			}
		}
		//sets all the sampler2Ds
		for (auto& uniform : m_samples2Ds) {
			for (int k = 0; k < uniform.second.size(); k++) {
				shader.setUniform(uniform.first + "[" + std::to_string(k) + "]", *uniform.second[k]);
			}
		}
	}

	//sets all the uniform values of the shader
	void Shader::setUniforms(Scene const * const scene, sf::RenderTarget& window, sf::FloatRect& viewBox) {
		//gets a set of lights
		std::vector<LightSource*> lights{ };
		scene->getLightSources(lights, viewBox);
		
		//loops through all lights
		std::map<std::string, int>typeIndexes;
		std::map<std::string, LightSourceInfo>typeInfos;
		UniformCollection uniforms;
		for (auto& light : lights) {
			//gets the name of the lightsource
			std::string name = getTypename(light);
			//checks if the lightsource is registered
			if (!hasType(name)) {
				throw std::out_of_range("The lightsource " + name + " was not registered to the shader");
			}
			//gets the index of this lightsource within its type
			int myIndex = 0;
			if (typeIndexes.find(name) != typeIndexes.end()) { myIndex = typeIndexes.at(name); }
			//sets extra uniforms from lightsource
			auto& lightUniforms = light->getUniforms();
			for (auto& uni : lightUniforms) {
				uniforms.addUniform(name, uni);
			}
			//sets position of lightsource
			if (typeInfos.find(name) == typeInfos.end()) {
				typeInfos.emplace(std::make_pair(name, LightSourceInfo()));
			}
			typeInfos.find(name)->second.positions.emplace_back(light->getFullWorldPos().getSfml());
			typeInfos.find(name)->second.colors.emplace_back(light->m_lightColor.getR(), light->m_lightColor.getG(), light->m_lightColor.getB());
			typeInfos.find(name)->second.intensities.emplace_back(light->m_intensity);

			//increments index for next of type
			if (typeIndexes.find(name) != typeIndexes.end()) { typeIndexes.find(name)->second += 1; }
			else { typeIndexes.emplace(std::make_pair(name, 1)); }

		}
		//sets the uniforms
		uniforms.setUniforms(m_shader);

		//sets amount for every light type
		for (auto& info : m_types) {
			m_shader.setUniform(info.name + "Amount", (typeIndexes.find(info.name) != typeIndexes.end() ? typeIndexes.at(info.name) : 0));
		}
		//sets all the previously registered positions
		for (auto& info : typeInfos) {
			m_shader.setUniformArray(info.first + "Pos", &(info.second.positions[0]), info.second.positions.size());
			m_shader.setUniformArray(info.first + "Color", &(info.second.colors[0]), info.second.colors.size());
			m_shader.setUniformArray(info.first + "Intensity", &(info.second.intensities[0]), info.second.intensities.size());
		}
		//sets ambient information
		m_shader.setUniform("ambientColor", sf::Glsl::Vec3((s_ambientColor.getVec() / 255.0f).getSfml()));
		m_shader.setUniform("ambientIntensity", s_ambientIntensity);
		
		//sets camera information
		m_shader.setUniform("viewBox", sf::Glsl::Vec4(viewBox.left, viewBox.top, viewBox.width, viewBox.height));
		m_shader.setUniform("resolution", sf::Glsl::Vec2(window.getSize().x, window.getSize().y));
		auto viewport = window.getView().getViewport();
		m_shader.setUniform("viewport", sf::Glsl::Vec4(viewport.left, viewport.top, viewport.width, viewport.height));
	}
	
	//draws a drawLayer
	void Shader::drawLayer(DrawLayer& layer, sf::RenderTarget& window) {
		//sets the textures as uniforms
		m_shader.setUniform("spriteMap", sf::Shader::CurrentTexture);
		m_shader.setUniform("normalMap", layer.m_normalTarget.getTexture());
		m_shader.setUniform("infoMap", layer.m_infoTarget.getTexture());

		//sets the renderStates
		sf::BlendMode blend = sf::BlendMode();
		sf::Transform tranf = sf::Transform();
		sf::RenderStates states{blend, tranf, &layer.m_diffuseTarget.getTexture(), &m_shader};

		//draws the layer
		sf::View oldView = window.getView();
		sf::View newView(sf::FloatRect(0, 0, layer.m_diffuseTarget.getSize().x, layer.m_diffuseTarget.getSize().y));
		window.setView(newView);
		window.draw(sf::Sprite(layer.m_diffuseTarget.getTexture()), states);
		window.setView(oldView);
	}

}