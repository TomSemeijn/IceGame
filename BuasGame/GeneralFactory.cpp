#include "GeneralFactory.h"

#include <reb/PixelParticle.h>
#include <reb/Particles/Fade.h>
#include <reb/Particles/MoveLine.h>
#include <reb/Particles/ColorFade.h>
#include <reb/ShadedSprite.h>
#include <reb/PointLight.h>
#include <reb/ParticleEmitter.h>
#include <reb/objParticle.h>
#include <reb/Particles/MoveDirShift.h>
#include <reb/Particles/TimedFade.h>
#include <reb/Particles/LightColorSync.h>
#include <reb/Particles/SpriteColorSync.h>

#include "TileIndex.h"
#include "LightPulseColor.h"
#include "LightPulseRadius.h"

#include "LayerIndex.h"

#include "HeatTile.h"
#include "PointHeat.h"


//returns a torch tile prototype
Tile* getTorchPrototype(Content* content, Level* level) {
	PixelParticle* flameParticle = new PixelParticle(Color(255, 0, 0));
	flameParticle->addComp(new Particles::Fade());

	flameParticle->addComp(new Particles::MoveLine(
		{ -120, -60 },
		{ 10, 20 }
	));
	//flameParticle->addComp(new ParticleMoveWhirl({ 10, 20 }, { 1, 2 }));
	flameParticle->addComp(new Particles::ColorFade(
		{
			Color(255, 0, 0),
			Color(255, 10, 0),
			Color(255, 30, 0),
			Color(255, 80, 0),
			Color(255, 200, 0),
			Color(255, 255, 0),
			Color(255, 255, 255)
		}
	));
	auto torchSprite = new ShadedSprite(content);
	torchSprite->setAlpha(1);
	HeatTile* prototype = new HeatTile(
		level,
		new PointHeat(150, 150)
	);
	prototype->getHeat()->setOfsset(Vector2(15.5, 6.5));
	prototype->setSprite(torchSprite);
	prototype->setLight(new PointLight(Color(255, 200, 30), 250, 150));
	prototype->setEmitter(new ParticleEmitter(
		flameParticle,
		100, 20,
		{ 0.25, 1.5 },
		{ Vector2{13, 10}, Vector2{19, 12} },
		{ 1, 3 },
		{ 0, PI },
		ParticleEmitter::PARTICLE_RENDER_BOTTOM
	));
	if (true) {
		prototype->addComponent(new LightPulseComp<PointLight>(0.05, 10, { -0.025, 0.025 }, 0.1, 5, { -0.025, 0.025 }));
		prototype->addComponent(new LightPulseColor(3, { -0.01, 0.01 }, {
			Color(255, 180, 30),
			Color(255, 200, 30),
			Color(255, 220, 30),
			}));
	}
	prototype->getLight()->setOffset(Vector3{ 15.5, 6.5, 5 });

	return prototype;
}

//adds a firefly emitter to the scene
GameObj* addFireflyEmitter(Scene& scene, Content* content, int flyAmount, std::pair<float, float> lifespanRange, std::pair<Vector2, Vector2> spawnRange) {
	GameObj* objEmitterObj = scene.addGameObj(new GameObj(new Transform(false, false, false)));

	BasicParticleObj* emittedObj = new BasicParticleObj();
	emittedObj->setLayer(LayerIndex::FIREFLYLAYER);
	emittedObj->setSprite(new ShadedSprite(content));
	emittedObj->setLight(new PointLight(Color(178, 255, 63), 200, 50));
	emittedObj->getLight()->setOffset(Vector3(3, 2.5, 6));
	emittedObj->getTransform()->setZ(-1);

	ObjParticle* objPart = new ObjParticle(emittedObj, true, Color(178, 255, 63, 120));
	float twelvethPI = PI / 12.0f;
	objPart->addComp(new Particles::MoveDirShift({ 0, PI }, { -twelvethPI, twelvethPI }, { 0.1, 1.5 }, { 10, 20 }));
	objPart->addComp(new Particles::TimedFade({
		{5, 0.5},
		{95, 0.5},
		{100, 0}
	}));

	objPart->addComp(new Particles::LightColorSync());
	objPart->addComp(new Particles::SpriteColorSync());
	objEmitterObj->setEmitter(
		new ParticleEmitter(
			objPart,
			flyAmount, flyAmount,
			lifespanRange,
			spawnRange,
			{ 1, 1 },
			{ 0, 0 }
		)
	);
	return objEmitterObj;
}