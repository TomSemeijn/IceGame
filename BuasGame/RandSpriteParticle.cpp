#include "RandSpriteParticle.h"

//default constructor
RandSpriteParticle::RandSpriteParticle(std::vector<reb::Content*> possibleSources, reb::Color color)
	: SpriteParticle{ possibleSources.at(reb::genRandi(0, possibleSources.size() - 1)), color }, m_possibleSources{ possibleSources } {};
//the constructor
RandSpriteParticle::RandSpriteParticle(float lifeTime, reb::Vector2 pos, float scale, float rotation, std::vector<reb::Content*> possibleSources, reb::Color color)
	: SpriteParticle{ lifeTime, pos, scale, rotation, possibleSources.at(reb::genRandi(0, possibleSources.size() - 1)), color }, m_possibleSources{ possibleSources } { };

//returns a clone of the particle with the given transform info
RandSpriteParticle* RandSpriteParticle::clone(float lifeTime, reb::Vector2 pos, float scale, float rotation) {
	auto newPart = new RandSpriteParticle(lifeTime, pos, scale, rotation, m_possibleSources, m_color);
	newPart->cloneComponentsFrom(this);
	return newPart;
};