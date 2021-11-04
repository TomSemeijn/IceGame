#include "RandSpriteParticleObject.h"

//constructor
RandSpriteParticleObject::RandSpriteParticleObject(std::vector<reb::Content*> possibleSources, reb::Transform* transform, BoundType boundType)
	:
	BasicParticleObj(transform, boundType),
	m_possibleSources{ possibleSources },
	m_mySource{ possibleSources.at(reb::genRandi(0, possibleSources.size() - 1)) }
{
	setRandSource();
};

//copy constructor
RandSpriteParticleObject::RandSpriteParticleObject(const RandSpriteParticleObject& other)
	:
	BasicParticleObj{ other },
	m_possibleSources{ other.m_possibleSources },
	m_mySource{ other.m_possibleSources.at(reb::genRandi(0, other.m_possibleSources.size() - 1)) }
{
	setRandSource();
};

//returns a clone of this object
RandSpriteParticleObject* RandSpriteParticleObject::clone() { return new RandSpriteParticleObject(*this); };

//sets the used source to the rigidbody and sprite
void RandSpriteParticleObject::setRandSource() {
	if (auto draw = getSprite()) {
		if (auto spr = dynamic_cast<reb::Sprite*>(draw)) {
			spr->setContent(m_mySource);
		}
	}
	if (auto body = getBody()) {
		body->animateShape(m_mySource);
	}
}