#pragma once
#include "LiquidEmitter.h"
#include <reb/ShadedSprite.h>
#include <reb/Rigidbody.h>
#include <reb/Concave.h>
#include "LavaObject.h"
#include "ContentIndex.h"

class ObsidianLiquid : public LiquidEmitter::LiquidParticle {
private:
	//the content that has the obsidian sprite and shape
	reb::Content* m_obsidianContent;

protected:
	//spawns an obsidian rock, despawns and despawns the other when touching a lava drop
	void onTouch(reb::Container* other, reb::Vector2 contactPoint, reb::Vector2 contactNormal)override;

public:
	//constructor
	ObsidianLiquid(reb::ContentLoader& content, float radius, float minDistance, reb::Material material, float gravity, reb::LayerIndex layer);

	//copy constructor
	ObsidianLiquid(const ObsidianLiquid& other);

	//returns a clone of the liquid particle
	virtual ObsidianLiquid* clone()override;
};