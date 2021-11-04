#pragma once
#include <reb/SpriteParticle.h>

//a particle that gets a random sprite from a given vector
class RandSpriteParticle : public reb::SpriteParticle {
private:
	//the possible sprite sources
	std::vector<reb::Content*> m_possibleSources;
public:
	//default constructor
	RandSpriteParticle(std::vector<reb::Content*> possibleSources, reb::Color color = reb::Color{ 255, 255, 255 });
	//the constructor
	RandSpriteParticle(float lifeTime, reb::Vector2 pos, float scale, float rotation, std::vector<reb::Content*> possibleSources, reb::Color color = reb::Color(255, 255, 255));

	//returns a clone of the particle with the given transform info
	virtual RandSpriteParticle* clone(float lifeTime, reb::Vector2 pos, float scale, float rotation)override;

};