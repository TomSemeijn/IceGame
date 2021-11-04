#pragma once
#include <reb/ObjParticle.h>
#include <reb/Content.h>
#include <reb/Sprite.h>
#include <reb/Rigidbody.h>

class RandSpriteParticleObject : public reb::BasicParticleObj {
protected:
	//the possible sprite sources
	std::vector<reb::Content*> m_possibleSources;

	//the source used by this object
	reb::Content* m_mySource;

	//sets the used source to the rigidbody and sprite
	void setRandSource();

public:
	//constructor
	RandSpriteParticleObject(std::vector<reb::Content*> possibleSources, reb::Transform* transform = new reb::Transform(true, true, true), BoundType boundType = BoundType::BOUNDS_RIGIDBODY);

	//copy constructor
	RandSpriteParticleObject(const RandSpriteParticleObject& other);

	//returns a clone of this object
	virtual RandSpriteParticleObject* clone()override;

};