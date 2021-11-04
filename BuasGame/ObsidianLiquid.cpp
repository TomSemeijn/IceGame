#include "ObsidianLiquid.h"

//constructor
ObsidianLiquid::ObsidianLiquid(reb::ContentLoader& content, float radius, float minDistance, reb::Material material, float gravity, reb::LayerIndex layer)
	:
	LiquidParticle{ radius, minDistance, material, gravity, layer },
	m_obsidianContent{ content.getContent(reb::ContentIndex::OBSIDIAN) }
{};

//copy constructor
ObsidianLiquid::ObsidianLiquid(const ObsidianLiquid& other)
	:
	LiquidParticle{ other },
	m_obsidianContent{ other.m_obsidianContent }
{};

//returns a clone of the liquid particle
ObsidianLiquid* ObsidianLiquid::clone() { return new ObsidianLiquid(*this); };

//spawns an obsidian rock, despawns and despawns the other when touching a lava drop
void ObsidianLiquid::onTouch(reb::Container* other, reb::Vector2 contactPoint, reb::Vector2 contactNormal) {
	if (typeid(*other) == typeid(LavaObject)) {
		//spawns an obsidian rock at the current position
		reb::GameObj* obsidian = getOwner()->addGameObj(new reb::GameObj(new reb::Transform(true, false, false, getTransform()->getPos(), 2)));
		reb::ShadedSprite* spr = new reb::ShadedSprite(m_obsidianContent);
		spr->setAlpha(5);
		obsidian->setSprite(spr);
		obsidian->setBody(new reb::Rigidbody(new reb::Concave(m_obsidianContent), reb::Material(10, 0, 1, 0.9), 100, true));

		//despawns itself and the lava drop
		destroy();
		other->destroy();
	}
}