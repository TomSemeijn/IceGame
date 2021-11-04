#pragma once
#include <reb/Tile.h>
#include "HeatHolder.h"

#include <reb/Drawable.h>
#include <reb/Rigidbody.h>
#include <reb/LightSource.h>
#include <reb/ParticleEmitter.h>

#include "HeatEmitter.h"

class HeatTile : public reb::Tile, public HeatHolder {
private:
	Level* m_copyLevel;
public:
	//constructor
	HeatTile(Level* level, HeatEmitter* heat);

	//copy constructor
	HeatTile(const HeatTile& other);

	//sets the level if not a prototype when the tile is added to a tileset
	void onOwnerSet()override;

	//returns a clone of the tile
	virtual HeatTile* getClone(reb::Tileset& set)override;

};