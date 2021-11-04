#include "LavaObject.h"

//constructor
LavaObject::LavaObject(Level* level, Iceblock* iceblock, float radius, float minDistance, reb::Material material, float gravity, reb::LayerIndex layer)
	: LiquidParticle{ radius, minDistance, material, gravity, layer }, HeatHolder{ level, nullptr }, m_iceblock{ iceblock } {};

//copy constructor
LavaObject::LavaObject(const LavaObject& other) : LiquidParticle{ other }, HeatHolder{ other }, m_iceblock{ other.m_iceblock } {};

//returns a clone of the liquid particle
LavaObject* LavaObject::clone() { return new LavaObject(*this); };

//gets called when the rigidbody (if solid) touches another solid rigidbody (behavior can be added by child classes)
void LavaObject::onTouch(reb::Container* other, reb::Vector2 contactPoint, reb::Vector2 contactNormal) {
	if (auto iceblock = dynamic_cast<Iceblock*>(other)) {
		if (iceblock == m_iceblock) {
			LavaHeat::s_touchingIce = true;
		}
	}
};