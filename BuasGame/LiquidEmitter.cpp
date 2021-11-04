#include "LiquidEmitter.h"
#include <fstream>
#include <reb/Rigidbody.h>
#include <reb/Circle.h>
#include <reb/Functions.h>
#include <reb/objParticle.h>
#include <reb/Particles/TimedFade.h>
#include <reb/DrawBatch.h>
#include "OpacityScaleSync.h"
#include "DespawnBounds.h"
#include "ReversePosSync.h"

//================================= drop ===============================================================

//drop constructor
LiquidEmitter::LiquidParticle::LiquidParticle(float radius, float minDistance, reb::Material material, float gravity, reb::LayerIndex layer)
	: m_minDistance(minDistance), BasicParticleObj{}
{
	//sets the rigidbody
	setBody(
		new reb::Rigidbody(
			new reb::Circle(reb::Vector2(radius / 2, radius / 2), radius),
			material,
			gravity,
			true
		)
	);
	//sets the layer
	setLayer(layer);
}

//copy constructor
LiquidEmitter::LiquidParticle::LiquidParticle(const LiquidParticle& other)
	: BasicParticleObj{ other }, m_minDistance(other.m_minDistance)
{}

//returns true if this drop can collide with the other container
//bool LiquidEmitter::LiquidParticle::canCollide(Container* other) {
	//return reb::withinVector(m_collisionLayers, other->getLayer());
	//reb::LayerIndex otherLayer = other->getLayer();
	//return otherLayer == reb::LayerIndex::WATER || otherLayer == reb::LayerIndex::TERRAIN;
//}

//returns the center of the particle
reb::Vector2 LiquidEmitter::LiquidParticle::getWorldCenter() {
	reb::Vector2 center{};
	if (auto body = getBody()) {
		center = getTransform()->getWorldSpace(body->getShape()->getCOM());
	}
	return center;
}

//================================= scale sync ============================================================

//syncs the values
void LiquidEmitter::ScaleDropSync::update(float elapsed) {
	if (hasObjOwner()) {
		//gets the owner and cast object
		reb::ObjParticle* owner = getObjOwner();
		LiquidParticle* object = (LiquidParticle*)owner->getObject();

		//sets initial values if that hadnt been done before
		if (!m_gotInitial || m_initialOwner != object) {
			if (object == nullptr) {
				m_initialOwner = nullptr;
				m_gotInitial = false;
			}
			else {
				m_initialOwner = object;
				m_gotInitial = true;
				m_initialMinDistance = object->m_minDistance;
			}
		}
		
		//skips if initial values still havent been set
		if (!m_gotInitial) { return; }

		//gets the scale
		float scale = owner->m_transform.getScale();
		//syncs the scale
		object->m_minDistance = m_initialMinDistance * scale;
		object->getTransform()->setScale(scale);
	}
}

//clones the component
LiquidEmitter::ScaleDropSync* LiquidEmitter::ScaleDropSync::clone() {
	return new ScaleDropSync(*this);
}

//================================= emitter ============================================================

//constructor
LiquidEmitter::LiquidEmitter(
	std::pair<float, float> lifetimeRange,
	reb::Color liquidColor,
	int dropAmount,
	int maxPerUpdate,
	float minDropDistance,
	std::pair<reb::Vector2, reb::Vector2> spawnArea,
	reb::Rect<float> liquidBounds,
	float dropRadius,
	reb::Material dropMaterial,
	float dropGravity,
	reb::LayerIndex layer,
	int alpha)
	
	:	
		reb::ParticleEmitter(
			new EditableObjParticle(
				new LiquidParticle(dropRadius, minDropDistance, dropMaterial, dropGravity, layer),
				false,
				reb::Color(255, 255, 255)
			), 
			dropAmount, maxPerUpdate,
			lifetimeRange,
			spawnArea,
			{1, 1},
			{0, 0}
		),
		m_liquidColor{ liquidColor },
		m_minDistance{ minDropDistance },
		m_alpha{ alpha },
		m_radius{ dropRadius }
{
	//adds reversePosSync component to the prototype
	m_prototype->addComp(new ReversePosSync());

	//adds a despawnBounds component to the prototype
	m_prototype->addComp(new DespawnBounds(liquidBounds));

	//adds a timed fade and opacity->scale sync to make the drops appear and disappear more smoothly
	m_prototype->addComp(new reb::Particles::TimedFade({
		{2.5, 1}, {97.5, 1}, {100, 0}
	}));
	m_prototype->addComp(new OpacityScaleSync());
	
	//adds a component that syncs the scale made by the timed fade and opacity->scale sync with the mindistance
	//and drop size
	m_prototype->addComp(new ScaleDropSync());

	//sets up height shader
	{
		//loads the shader template to a string (https://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c)
		std::ifstream ifs(SHADERDIR_H);
		std::string shaderTempl((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));

		//replaces the dropamount substring with the actual dropamount
		reb::replaceSubstr(shaderTempl, DROPSUBSTR, std::to_string(dropAmount));

		//loads the shader from the filled in template string
		m_liquidShader_h.loadFromMemory(shaderTempl, sf::Shader::Type::Fragment);

		//sets the static uniforms to the shader
		m_liquidShader_h.setUniform("dropCount", m_lastDropCount);
	}

	//sets up normal shader
	{
		//loads the shader template to a string (https://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c)
		std::ifstream ifs(SHADERDIR_N);
		std::string shaderTempl((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));

		//replaces the dropamount substring with the actual dropamount
		reb::replaceSubstr(shaderTempl, DROPSUBSTR, std::to_string(dropAmount));

		//loads the shader from the filled in template string
		m_liquidShader_n.loadFromMemory(shaderTempl, sf::Shader::Type::Fragment);
	}

	//sets up diffuse shader
	{
		//loads the shader template to a string (https://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c)
		std::ifstream ifs(SHADERDIR);
		std::string shaderTempl((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));

		//replaces the dropamount substring with the actual dropamount
		reb::replaceSubstr(shaderTempl, DROPSUBSTR, std::to_string(dropAmount));

		//loads the shader from the filled in template string
		m_liquidShader.loadFromMemory(shaderTempl, sf::Shader::Type::Fragment);

		//sets the static uniforms to the shader
		m_liquidShader.setUniform("dropColor", liquidColor.getGlsl());
		m_lastColor = liquidColor;
	}
	
}

//draws all the particles to the screen
void LiquidEmitter::draw(reb::Renderer& renderer, const sf::FloatRect& viewBox) {

	//sets up a vector for all the particle centers
	m_centers.clear();
	std::vector<float> minDistances{};
	minDistances.reserve(MAXPARTICLES);

	//adds all the particle centers to the vector
	for (auto& part : m_particles) {
		reb::ObjParticle* objPart = (reb::ObjParticle*)part;
		if (LiquidParticle* liquidPart = (LiquidParticle*)objPart->getObject()) {
			m_centers.push_back(liquidPart->getWorldCenter().getSfml());
			minDistances.push_back(liquidPart->m_minDistance);
		}
	}
	int dropCount = m_centers.size();

	//sets the dropCenters uniform
	m_liquidShader_h.setUniformArray("dropCenters", m_centers.data(), dropCount);
	m_liquidShader_h.setUniformArray("minDistance", minDistances.data(), dropCount);

	//checks if the dropcolor has changed
	if (m_liquidColor != m_lastColor) {
		//resets the uniform
		m_lastColor = m_liquidColor;
		m_liquidShader.setUniform("dropColor", m_lastColor.getGlsl());
	}

	//checks if the dropcount has changed
	if (dropCount != m_lastDropCount) {
		m_lastDropCount = dropCount;
		m_liquidShader_h.setUniform("dropCount", m_lastDropCount);
	}

	//sets the viewbox uniform
	m_liquidShader_h.setUniform("viewBox", sf::Glsl::Vec4(viewBox.left, viewBox.top, viewBox.width, viewBox.height));
	m_liquidShader_n.setUniform("resolution", sf::Glsl::Vec2(viewBox.width, viewBox.height));

	//creates a quad to serve as a canvas
	sf::VertexArray quad{ sf::PrimitiveType::Quads, 4 };
	quad[0] = sf::Vertex{ sf::Vector2f{viewBox.left, viewBox.top}, sf::Color::White };
	quad[1] = sf::Vertex{ sf::Vector2f{viewBox.left + viewBox.width, viewBox.top}, sf::Color::White };
	quad[2] = sf::Vertex{ sf::Vector2f{viewBox.left + viewBox.width, viewBox.top + viewBox.height}, sf::Color::White };
	quad[3] = sf::Vertex{ sf::Vector2f{viewBox.left, viewBox.top + viewBox.height}, sf::Color::White };

	//draws on height
	{
		//creates renderstates for the heightmap texture
		sf::RenderStates states{ &m_liquidShader_h };

		//draws the diffuse texture
		if (m_lastViewSize_h != reb::Vector2(viewBox.width, viewBox.height)) {
			m_lastViewSize_h = reb::Vector2(viewBox.width, viewBox.height);
			m_heightTexture.create((unsigned int)viewBox.width, (unsigned int)viewBox.height);
		}
		m_heightTexture.clear(sf::Color(0, 0, 0, 0));
		m_heightTexture.setView(sf::View(viewBox));
		m_heightTexture.draw(quad, states);
		m_heightTexture.display();
		//m_heightTexture.getTexture().copyToImage().saveToFile("Content/heightmap.png");
	}

	quad[0].texCoords.x = 0;
	quad[0].texCoords.y = (unsigned int)viewBox.height;

	quad[1].texCoords.x = (unsigned int)viewBox.width;
	quad[1].texCoords.y = (unsigned int)viewBox.height;

	quad[2].texCoords.x = (unsigned int)viewBox.width;
	quad[2].texCoords.y = 0;

	quad[3].texCoords.x = 0;
	quad[3].texCoords.y = 0;

	//draws on diffuse
	{
		//creates renderstates for the diffuse texture
		sf::Transform temp{};
		sf::BlendMode blendTemp{};
		sf::RenderStates states{ blendTemp, temp, &m_heightTexture.getTexture(), &m_liquidShader };

		//draws the diffuse texture
		if (m_lastViewSize != reb::Vector2(viewBox.width, viewBox.height)) {
			m_lastViewSize = reb::Vector2(viewBox.width, viewBox.height);
			m_diffuseTexture.create((unsigned int)viewBox.width, (unsigned int)viewBox.height);
		}
		m_diffuseTexture.clear(sf::Color(0, 0, 0, 0));
		m_diffuseTexture.setView(sf::View(viewBox));
		m_diffuseTexture.draw(quad, states);
		m_diffuseTexture.display();
		//m_diffuseTexture.getTexture().copyToImage().saveToFile("Content/diffusemap.png");

	}
	
	//draws on normal
	{
		//creates renderstates for the diffuse texture
		sf::Transform temp{};
		sf::BlendMode blendTemp{};
		sf::RenderStates states{ blendTemp, temp, &m_heightTexture.getTexture(), &m_liquidShader_n };

		//draws the diffuse texture
		if (m_lastViewSize_n != reb::Vector2(viewBox.width, viewBox.height)) {
			m_lastViewSize_n = reb::Vector2(viewBox.width, viewBox.height);
			m_normalTexture.create((unsigned int)viewBox.width, (unsigned int)viewBox.height);
		}
		m_normalTexture.clear(sf::Color(0, 0, 0, 0));
		m_liquidShader_n.setUniform("unit_wave", sf::Shader::CurrentTexture);
		m_normalTexture.setView(sf::View(viewBox));
		m_normalTexture.draw(quad, states);
		m_normalTexture.display();
		//m_normalTexture.getTexture().copyToImage().saveToFile("Content/normalmap.png");
	}

	//sets up a drawInfo
	reb::DrawInfo drawInfo{};
	drawInfo.m_alpha = m_alpha;
	drawInfo.m_boundingBox = viewBox;
	drawInfo.m_cells = 255;
	drawInfo.m_diffuseTexture = const_cast<sf::Texture*>(&m_diffuseTexture.getTexture());
	drawInfo.m_normalTexture = const_cast<sf::Texture*>(&m_normalTexture.getTexture());
	drawInfo.m_rotation = 0;
	drawInfo.m_shaded = true;
	drawInfo.m_transparent = true;
	drawInfo.m_verts.emplace_back(sf::Vector2f(viewBox.left, viewBox.top), sf::Vector2f(0, 0));
	drawInfo.m_verts.emplace_back(sf::Vector2f(viewBox.left + viewBox.width, viewBox.top), sf::Vector2f(viewBox.width, 0));
	drawInfo.m_verts.emplace_back(sf::Vector2f(viewBox.left + viewBox.width, viewBox.top + viewBox.height), sf::Vector2f(viewBox.width, viewBox.height));
	drawInfo.m_verts.emplace_back(sf::Vector2f(viewBox.left, viewBox.top + viewBox.height), sf::Vector2f(0, viewBox.height));
	drawInfo.m_z = getOwner()->getTransform()->getZ();

	//draws
	renderer.draw(drawInfo);
}

//returns the prototype object so it can be modified
LiquidEmitter::LiquidParticle* LiquidEmitter::getPrototype() {
	return (LiquidParticle*)((EditableObjParticle*)m_prototype)->getObjPrototype();
}

//returns a const reference to the particle vector
const std::vector<reb::Particle*>& LiquidEmitter::getParticles()const {
	return m_particles;
}

//returns the minDistance
float LiquidEmitter::getMinDistance()const {
	return m_minDistance;
}

//replaces the prototype completely
void LiquidEmitter::setPrototype(LiquidParticle* newProto) {
	((EditableObjParticle*)m_prototype)->setObjPrototype(newProto);
}

//returns the current centers (updated on draw)
std::vector<sf::Vector2f>& LiquidEmitter::getCenters() {
	return m_centers;
}