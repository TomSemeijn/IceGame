#include "EditableObjParticle.h"

//default constructor
EditableObjParticle::EditableObjParticle(reb::BasicParticleObj* prototype, bool syncPos, reb::Color color)
	: reb::ObjParticle{ prototype, syncPos, color }{};
//the constructor
EditableObjParticle::EditableObjParticle(float lifeTime, reb::Vector2 pos, float scale, float rotation, reb::BasicParticleObj* prototype, bool syncPos, reb::Color color)
	: reb::ObjParticle{ lifeTime, pos, scale, rotation, prototype, syncPos, color }{};

//returns the object prototype
reb::BasicParticleObj* EditableObjParticle::getObjPrototype() { return m_prototype; };

//replaces the object prototype
void EditableObjParticle::setObjPrototype(reb::BasicParticleObj* newProto) {
	if (m_prototype != nullptr) {
		delete m_prototype;
	}
	m_prototype = newProto;
}