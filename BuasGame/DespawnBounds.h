#pragma once
#include <reb/ParticleComp.h>
#include <reb/ParticleEmitter.h>

#include <reb/Sprite.h>
#include <reb/ObjParticle.h>

class DespawnBounds : public reb::ParticleComp {
private:
	//the bounds the particle should stay within
	reb::Rect<float> m_bounds;

protected:
	//despawns the owner when it goes out of bounds
	void update(float elapsed)override;

	//returns a clone of the component
	DespawnBounds* clone()override;
public:
	//constructor
	DespawnBounds(reb::Rect<float> bounds) : m_bounds{ bounds } {};

};