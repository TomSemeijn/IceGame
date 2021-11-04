#pragma once
#include <reb/ParticleEmitter.h>
#include <reb/Particles/ObjSync.h>
#include <reb/Material.h>
#include <SFML/Graphics.hpp>
#include "EditableObjParticle.h"
#include "LayerIndex.h"

//a particle emitter that emits object particles with circular rigidbodies
//and draws them in a way that makes them look like a liquid
class LiquidEmitter : public reb::ParticleEmitter {

public:
	//an object to be passed to an object particle that has a circular rigidbody with the given radius, restitution
	//and density and collides with only the given layers, this object will act as a drop of the liquid
	class LiquidParticle : public reb::BasicParticleObj {
	public:
		//the mindistance of this drop
		float m_minDistance;

		//constructor
		LiquidParticle(float radius, float minDistance, reb::Material material, float gravity, reb::LayerIndex layer);

		//copy constructor
		LiquidParticle(const LiquidParticle& other);

		//returns a clone of the liquid particle
		virtual BasicParticleObj* clone()override { return new LiquidParticle(*this); };

		//returns the center of the particle
		reb::Vector2 getWorldCenter();
	};

private:

	//a component that syncs the particle scale with the radius and mindistance of a liquid drop
	class ScaleDropSync : public reb::Particles::ObjSync {
	private:
		//initial values
		float m_initialMinDistance;
		bool m_gotInitial = false;
		LiquidParticle* m_initialOwner = nullptr;
	public:
		//syncs the values
		void update(float elapsed)override;

		//clones the component
		ScaleDropSync* clone()override;
	};

	//the prototype object the particles are based on
	LiquidParticle* m_objPrototype;

	//the color of the liquid
	reb::Color m_liquidColor;

	//the alpha value of the liquid
	int m_alpha;

	float m_radius;

	//the min distance for a position to be removed from a drop in order to be part of the liquid
	float m_minDistance;

	//the shader used to draw the liquid
	sf::Shader m_liquidShader;
	sf::Shader m_liquidShader_n;
	sf::Shader m_liquidShader_h;

	//the file location of the shader template
	const std::string SHADERDIR = "Content/Shaders/liquidShader.frag";
	const std::string SHADERDIR_N = "Content/Shaders/liquidNormal.frag";
	const std::string SHADERDIR_H = "Content/Shaders/liquidHeight.frag";

	//the substring in the shader template that should be replaced by the amount of drops
	const std::string DROPSUBSTR = "${DROPAMOUNT}";

	//the last used color
	reb::Color m_lastColor;
	int m_lastDropCount = 0;

	//the rendertextures for the diffuse and normal textures
	sf::RenderTexture m_heightTexture;
	sf::RenderTexture m_diffuseTexture;
	sf::RenderTexture m_normalTexture;
	reb::Vector2 m_lastViewSize;
	reb::Vector2 m_lastViewSize_n;
	reb::Vector2 m_lastViewSize_h;

	//all the centers in sfml vectors
	std::vector<sf::Vector2f> m_centers;

public:
	//constructor
	LiquidEmitter(
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
		int alpha = 0
	);

	//draws all the particles to the screen
	void draw(reb::Renderer& renderer, const sf::FloatRect& viewBox)override;

	//returns the prototype object so it can be modified
	LiquidParticle* getPrototype();

	//replaces the prototype completely
	void setPrototype(LiquidParticle* newProto);

	//returns a const reference to the particle vector
	const std::vector<reb::Particle*>& getParticles()const;

	//returns the minDistance
	float getMinDistance()const;

	//returns the current centers (updated on draw)
	std::vector<sf::Vector2f>& getCenters();

};
