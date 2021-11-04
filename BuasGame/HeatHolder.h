#pragma once
#include "Level.h"

class HeatEmitter;
class HeatTile;

//a heat object is an object with a function that raises the temperature around it
class HeatHolder {
	friend HeatTile;
private:
	HeatEmitter* m_heat;
	Level* m_level;
protected:
	//changes the level the holder belongs to
	void setLevel(Level* level);

	//copy constructor
	HeatHolder(const HeatHolder& other);
public:
	//constructor
	HeatHolder(Level* level, HeatEmitter* heat);

	//heat emitter functions
	void setHeatEmitter(HeatEmitter* heat);
	void clearHeatEmitter();
	
	//returns the heat emitter
	HeatEmitter* getHeat();

	//destructor
	virtual ~HeatHolder();
};