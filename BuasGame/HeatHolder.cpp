#include "HeatHolder.h"
#include "HeatEmitter.h"
#include "Level.h"

//constructor
HeatHolder::HeatHolder(Level* level, HeatEmitter* heat)
	:	m_heat(heat),
		m_level(level)
{
	//adds to the heat objects in the level
	if (m_level != nullptr) {
		level->m_heatObjects.push_back(this);
	}
	//sets this to the owner of the heat emitter
	if (m_heat != nullptr) {
		m_heat->setOwner(this);
	}
}

//copy constructor
HeatHolder::HeatHolder(const HeatHolder& other) : m_heat{ other.m_heat != nullptr ? other.m_heat->clone() : nullptr }, m_level{ other.m_level } {
	//adds to the heat objects in the level
	if (m_level != nullptr) {
		m_level->m_heatObjects.push_back(this);
	}
	//sets this to the owner of the heat emitter
	if (m_heat != nullptr) {
		m_heat->setOwner(this);
	}
}

//changes the level the holder belongs to
void HeatHolder::setLevel(Level* level) {

	//removes itself from the heat objects in the level
	if (m_level != nullptr) {
		reb::swapPopEl(m_level->m_heatObjects, this);
	}

	//changes the level
	m_level = level;

	//adds to the heat objects in the level
	if (m_level != nullptr) {
		level->m_heatObjects.push_back(this);
	}

}

//replaces the heat emitter
void HeatHolder::setHeatEmitter(HeatEmitter* heat) {
	//frees memory of current emitter if it exists
	if (m_heat != nullptr) {
		delete m_heat;
	}
	//sets the new emitter
	m_heat = heat;
	if (m_heat != nullptr) {
		m_heat->setOwner(this);
	}
}

//clears the heat emitter
void HeatHolder::clearHeatEmitter() {
	//frees memory if it exists
	if (m_heat != nullptr) {
		delete m_heat;
		m_heat = nullptr;
	}
}

//returns the heat emitter
HeatEmitter* HeatHolder::getHeat() {
	return m_heat;
}

//destructor
HeatHolder::~HeatHolder() {
	//frees memory of the heat emitter if it exists
	if (m_heat != nullptr) {
		delete m_heat;
	}
	//removes itself from the heat objects in the level
	if (m_level != nullptr) {
		reb::swapPopEl(m_level->m_heatObjects, this);
	}
}