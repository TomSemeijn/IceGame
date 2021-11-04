#pragma once
#include "LiquidEmitter.h"

//a liquid emitter with multiple spawn areas so the liquids will mix and they wont take up too many renderLayers and such
class MultiLiquidEmitter : public LiquidEmitter {
public:
	//struct for a spawning area
	struct SpawningArea {

		std::vector<reb::Particle*> m_particles;

		int m_maxParticles;
		std::pair<reb::Vector2, reb::Vector2> m_area;
		std::pair<float, float> m_lifeTimeRange;
		int m_minSpawn;

		SpawningArea(std::pair<reb::Vector2, reb::Vector2> area, std::pair<float, float> lifeTimeRange, int maxParticles, int minSpawn = 0) : m_area{ area }, m_maxParticles{ maxParticles }, m_lifeTimeRange{ lifeTimeRange }, m_minSpawn{ minSpawn }{};

	};

private:
	//a special prototype that doesnt have scaling
	reb::Particle* m_scalelessPrototype;

	//all the spawning areas belonging to this emitter
	std::vector<SpawningArea> m_spawns;

	//the amount that needs to be spawned
	int m_toSpawn;
	int m_currentToSpawn;

protected:
	//removes a despawned particle from the vector of the spawning area it belonged to
	void onDespawn(reb::Particle* particle)override;

	//spawns a particle in the first spawning area that needs one
	void spawnParticle()override;

	//returns false if the first spawning area that needs spawns isnt missing enough particles yet
	bool shouldSpawn()override;

	//sets the current toSpawn amount before an update
	void preUpdate(float elapsed)override;

public:
	//constructor
	MultiLiquidEmitter(
		std::vector<SpawningArea> spawns,
		reb::Color liquidColor,
		int dropAmount,
		float minDropDistance,
		reb::Rect<float> liquidBounds,
		float dropRadius,
		reb::Material dropMaterial,
		float dropGravity,
		reb::LayerIndex layer,
		int alpha = 0
	);

};