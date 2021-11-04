#include "LiquidGlow.h"
#include <reb/Container.h>
#include "LiquidEmitter.h"

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
std::string LiquidGlow::getLightFunc(std::string lightName)const {
	return
"	vec2 sampleCoord = (pos.xy - viewBox.xy) / viewBox.ba;\n\
	vec4 sampleColor = texture2D(GlowMap, sampleCoord);\n\
	float attenuation = sampleColor.a;\n\
	sampleColor.y = 1.0 - sampleColor.y;\n\
	if(attenuation == 0){ return vec3(0.0, 0.0, 0.0); }\n\
	return mainLighting(diffuseColor, lightColor, lightIntensity * attenuation, normalize(vec3(sampleColor.xy, (lightPos.z - pos.z) / abs(max(lightPos.z, pos.z)))), normal, attenuation);\n";
}

//called before uniforms are returned to give the lightsource a chance to update things if necessary
void LiquidGlow::updateUniforms() {
	//verifies there is an owner
	if (hasOwner()) {
		//gets the owner and verifies it has a particle emitter
		auto owner = getOwner();
		if (auto emitter = owner->getEmitter()) {
			//verifies the emitter is a liquid emitter and casts it
			if (LiquidEmitter* castEmitter = dynamic_cast<LiquidEmitter*>(emitter)) {
				
				//rebuilds the shader if the dropAmount isnt correct
				if (m_lastCenterAmount != castEmitter->MAXPARTICLES) {
					m_lastCenterAmount = castEmitter->MAXPARTICLES;
					rebuildShader(castEmitter->MAXPARTICLES);
				}

				//recreates the rendertexture if necessary
				if (m_glowMap.getSize().x != (unsigned int)m_viewBox.width || m_glowMap.getSize().y != (unsigned int)m_viewBox.height) {
					m_glowMap.create((unsigned int)m_viewBox.width, (unsigned int)m_viewBox.height);
				}

				//recreates the rendertexture if necessary
				if (m_normalMap.getSize().x != (unsigned int)m_viewBox.width || m_normalMap.getSize().y != (unsigned int)m_viewBox.height) {
					m_normalMap.create((unsigned int)m_viewBox.width, (unsigned int)m_viewBox.height);
				}

				//sets uniforms in the map shader
				std::vector<sf::Vector2f>& centers = castEmitter->getCenters();
				if (!centers.empty()) {
					m_mapShader.setUniformArray("dropCenters", centers.data(), centers.size());
				}
				m_mapShader.setUniform("viewBox", sf::Glsl::Vec4(m_viewBox.left, m_viewBox.top, m_viewBox.width, m_viewBox.height));
				m_mapShader.setUniform("minDistance", m_radius);
				m_mapShader.setUniform("dropCount", (int)centers.size());

				//re-draws the glow map
				sf::VertexArray verts{ sf::PrimitiveType::Quads, 4 };
				verts[0] = sf::Vertex(sf::Vector2f(0, 0));
				verts[1] = sf::Vertex(sf::Vector2f(m_viewBox.width, 0));
				verts[2] = sf::Vertex(sf::Vector2f(m_viewBox.width, m_viewBox.height));
				verts[3] = sf::Vertex(sf::Vector2f(0, m_viewBox.height));
				m_glowMap.clear(sf::Color(0, 0, 0, 0));
				m_glowMap.draw(verts, &m_mapShader);
				m_glowMap.display();

				//re-draws the normal map
				verts[0].texCoords = sf::Vector2f(0, 0);
				verts[1].texCoords = sf::Vector2f(m_viewBox.width, 0);
				verts[2].texCoords = sf::Vector2f(m_viewBox.width, m_viewBox.height);
				verts[3].texCoords = sf::Vector2f(0, m_viewBox.height);

				sf::Transform temp{};
				sf::BlendMode blendTemp{};
				sf::RenderStates states{ blendTemp, temp, &m_glowMap.getTexture(), &m_normalShader };

				m_normalShader.setUniform("unit_wave", sf::Shader::CurrentTexture);
				m_normalShader.setUniform("resolution", sf::Vector2f(m_viewBox.width, m_viewBox.height));

				m_normalMap.clear(sf::Color(0, 0, 0, 0));
				m_normalMap.draw(verts, states);
				m_normalMap.display();

				//re-sets the uniform
				setUniform(reb::Uniform{ "GlowMap", &m_normalMap.getTexture() });

				//m_glowMap.getTexture().copyToImage().saveToFile("Content/temp.png");

			}
		}
	}
}

//returns true if the lightsource has any effect within the given viewbox
bool LiquidGlow::onScreen(sf::FloatRect& viewBox)const {
	m_viewBox = viewBox;
	return true;
}

//rebuilds the shader
void LiquidGlow::rebuildShader(int dropAmount) {

	{
		//loads the shader template to a string (https://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c)
		std::ifstream ifs("Content/Shaders/liquidGlow.frag");
		std::string shaderTempl((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));

		//replaces the dropamount substring with the actual dropamount
		reb::replaceSubstr(shaderTempl, "${DROPAMOUNT}", std::to_string(dropAmount));

		//sets as shader
		if (!m_mapShader.loadFromMemory(shaderTempl, sf::Shader::Fragment)) {
			throw std::out_of_range("Couldn't load the liquid glow shader");
		}
	}
	
	{
		//loads the shader template to a string (https://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c)
		std::ifstream ifs("Content/Shaders/liquidGlowDirectional.frag");
		std::string shaderTempl((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));

		//replaces the dropamount substring with the actual dropamount
		reb::replaceSubstr(shaderTempl, "${DROPAMOUNT}", std::to_string(dropAmount));

		//sets as shader
		if (!m_normalShader.loadFromMemory(shaderTempl, sf::Shader::Fragment)) {
			throw std::out_of_range("Couldn't load the liquid normal shader");
		}
	}

}

//returns a clone
LiquidGlow* LiquidGlow::clone()const {
	return new LiquidGlow(m_lightColor, m_intensity, m_radius);
}

//constructor
LiquidGlow::LiquidGlow() : m_glowMap{}, reb::LightSource{ {reb::Uniform{"GlowMap", &m_glowMap.getTexture()} } } {

}

LiquidGlow::LiquidGlow(reb::Color color, float Intensity, float radius) : reb::LightSource{ color, Intensity }, m_radius{ radius } {
	setUniform(reb::Uniform{ "GlowMap", &m_glowMap.getTexture() });
}