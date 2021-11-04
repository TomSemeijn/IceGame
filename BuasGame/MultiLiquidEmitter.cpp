#include "MultiLiquidEmitter.h"
#include "ReversePosSync.h"
#include "DespawnBounds.h"

//constructor
MultiLiquidEmitter::MultiLiquidEmitter(
	std::vector<SpawningArea> spawns,
	reb::Color liquidColor,
	int dropAmount,
	float minDropDistance,
	reb::Rect<float> liquidBounds,
	float dropRadius,
	reb::Material dropMaterial,
	float dropGravity,
	reb::LayerIndex layer,
	int alpha)
	:
	LiquidEmitter(	spawns.front().m_lifeTimeRange, liquidColor, dropAmount, dropAmount, minDropDistance, spawns.front().m_area,
					liquidBounds, dropRadius, dropMaterial, dropGravity, layer, alpha),
	m_spawns{ spawns }, m_toSpawn{ dropAmount }, m_currentToSpawn{ m_toSpawn }
{}

//sets the current toSpawn amount before an update
void MultiLiquidEmitter::preUpdate(float elapsed) {
	m_currentToSpawn = m_toSpawn;
}

//removes a despawned particle from the vector of the spawning area it belonged to
void MultiLiquidEmitter::onDespawn(reb::Particle* particle) {
	for (auto& spawn : m_spawns) {
		auto iter = std::find(spawn.m_particles.begin(), spawn.m_particles.end(), particle);
		if (iter != spawn.m_particles.end()) {
			reb::swapPopIter(spawn.m_particles, iter);
			break;
		}
	}
	m_toSpawn++;
}

//spawns a particle in the first spawning area that needs one
void MultiLiquidEmitter::spawnParticle() {
	//verifies there is an owner
	if (hasOwner()) {
		for (auto& spawn : m_spawns) {
			if (spawn.m_particles.size() < spawn.m_maxParticles) {
				auto owner = getOwner();
				//creates the new particle
				auto clone = m_prototype->clone(
					reb::genRandf(spawn.m_lifeTimeRange.first, spawn.m_lifeTimeRange.second),
					owner->getTransform()->getWorldSpace(
						reb::Vector2{
							reb::genRandf(spawn.m_area.first.x, spawn.m_area.second.x),
							reb::genRandf(spawn.m_area.first.y, spawn.m_area.second.y)
						}
					),
					reb::genRandf(getScaleRange().first, getScaleRange().second),
					reb::genRandf(getRotationRange().first, getRotationRange().second)
				);
				addParticle(clone);
				spawn.m_particles.push_back(clone);
				m_toSpawn--;
				return;
			}
		}
	}
}

//returns false if the first spawning area that needs spawns isnt missing enough particles yet
bool MultiLiquidEmitter::shouldSpawn() {
	for (auto& spawn : m_spawns) {
		if (spawn.m_particles.size() < spawn.m_maxParticles) {
			if (m_currentToSpawn >= spawn.m_minSpawn) { return true; }
			else { return false; }
		}
	}
	return false;
}

