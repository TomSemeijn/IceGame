#pragma once
#include "LiquidEmitter.h"
#include "Iceblock.h"
#include "LavaHeat.h"
#include "HeatHolder.h"

//a liquid particle that sets the static boolean in the lava heat emitter to true when it touches the iceblock
class LavaObject : public LiquidEmitter::LiquidParticle, public HeatHolder {
private:
	//a pointer to the iceblock
	Iceblock* m_iceblock;
public:
	//constructor
	LavaObject(Level* level, Iceblock* iceblock, float radius, float minDistance, reb::Material material, float gravity, reb::LayerIndex layer);

	//copy constructor
	LavaObject(const LavaObject& other);

	//returns a clone of the liquid particle
	virtual LavaObject* clone()override;

	//gets called when the rigidbody (if solid) touches another solid rigidbody (behavior can be added by child classes)
	virtual void onTouch(reb::Container* other, reb::Vector2 contactPoint, reb::Vector2 contactNormal)override;

};