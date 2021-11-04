#pragma once
#include <reb/objParticle.h>

//an extension of objparticle just so the prototype of the object can be publically available
class EditableObjParticle : public reb::ObjParticle {
public:
	//default constructor
	EditableObjParticle(reb::BasicParticleObj* prototype, bool syncPos = true, reb::Color color = reb::Color{ 255, 255, 255 });
	//the constructor
	EditableObjParticle(float lifeTime, reb::Vector2 pos, float scale, float rotation, reb::BasicParticleObj* prototype, bool syncPos = true, reb::Color color = reb::Color(255, 255, 255));

	//returns the object prototype
	reb::BasicParticleObj* getObjPrototype();

	//replaces the object prototype
	void setObjPrototype(reb::BasicParticleObj* newProto);

};