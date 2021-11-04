#include "Penguin.h"
#include <reb/ShadedSprite.h>
#include <reb/LoopAnimator.h>
#include <reb/Concave.h>
#include <reb/PointLight.h>
#include <reb/ParticleEmitter.h>
#include <reb/PixelParticle.h>
#include <reb/Particles/MoveLine.h>
#include <reb/Particles/Fade.h>
#include <reb/Particles/ColorFade.h>

#include "PointHeat.h"
#include "LightPulseColor.h"
#include "LightPulseRadius.h"

#include "ContentIndex.h"
#include "LayerIndex.h"

//constructor
Penguin::Penguin(Level* level, reb::Scene& scene, reb::ContentLoader& content, reb::Vector2 pos) 
	: HeatObj(
		level, 
		nullptr, 
		new reb::Transform(true, false, false, pos)
		//,reb::Container::BOUNDS_SPRITE
	) {
	
	//adds the animator
	auto anim = scene.addAnimator(new reb::LoopAnimator({
		{content.getContent(reb::PENGUIN_1), 0.2},
		{content.getContent(reb::PENGUIN_0), 0.2},
		{content.getContent(reb::PENGUIN_2), 0.2},
		{content.getContent(reb::PENGUIN_0), 0.2},
	}, true, true, reb::Animator::STEADY_BOTTOMMIDDLE));
	//adds itself to the animator
	anim->addObject(this);

	//adds the sprite
	setSprite(new reb::ShadedSprite(content.getContent(reb::PENGUIN_0)));

	//adds the rigidbody
	setBody(new reb::Rigidbody(new reb::Convex(content.getContent(reb::PENGUIN_0)), reb::Material(0.5, 0.1, 1, 1), 60, true));

	//adds the lightsource
	setLight(new reb::PointLight(reb::Color(255, 200, 30), 250, 150));
	getLight()->setOffset(reb::Vector3(6.5, 9.5, 10));

	//adds pulsing components for the radius, intensity and color of the light
	addComponent(new LightPulseComp<reb::PointLight>(0.05, 10, { -0.025, 0.025 }, 0.1, 5, { -0.025, 0.025 }));
	addComponent(new LightPulseColor(3, { -0.01, 0.01 }, {
		reb::Color(255, 180, 30),
		reb::Color(255, 200, 30),
		reb::Color(255, 220, 30),
	}));

	//adds the particle emitter
	{
		//sets up the particle prototype
		reb::PixelParticle* flameParticle = new reb::PixelParticle(reb::Color(255, 0, 0));
		flameParticle->addComp(new reb::Particles::Fade());

		flameParticle->addComp(new reb::Particles::MoveLine(
			{ -120, -60 },
			{ 10, 20 }
		));
		flameParticle->addComp(new reb::Particles::ColorFade(
			{
				reb::Color(255, 0, 0),
				reb::Color(255, 10, 0),
				reb::Color(255, 30, 0),
				reb::Color(255, 80, 0),
				reb::Color(255, 200, 0),
				reb::Color(255, 255, 0),
				reb::Color(255, 255, 255)
			}
		));
		//sets the emitter
		setEmitter(new reb::ParticleEmitter(
			flameParticle,
			100, 20,
			{ 0.25, 1.5 },
			{ reb::Vector2{6, 9}, reb::Vector2{7, 10} },
			{ 1, 3 },
			{ 0, reb::PI }
		));
	}

	//adds the heat emitter (need to be here instead of the initializer list because the constants need to be set)
	setHeatEmitter(new PointHeat(HEATRADIUS, HEATAMOUNT));
	getHeat()->setOfsset(reb::Vector2(6.5, 9.5));

	//sets the layer
	setLayer(reb::PENGUINLAYER);

	//sets initial flip
	getTransform()->setFlipX(true);

}

//makes it so it only collides with the terrain and other penguins
bool Penguin::canCollide(reb::Container* other) {
	return other->getLayer() == reb::TERRAIN || typeid(*other) == typeid(Penguin);
}

//gets position on early update
void Penguin::earlyUpdate(float elapsed) {
	m_lastPos = getTransform()->getPos();
}

//moves on late update
void Penguin::lateUpdate(float elapsed) {
	//switches direction if the penguin didnt move enough
	if ((m_lastPos - getTransform()->getPos()).getLength() < MINMOVE * elapsed) {
		getTransform()->toggleFlipX();
		m_currentDir *= -1;
	}
	//sets the x velocity to move
	getBody()->setVelocity(reb::Vector2(MOVESPEED * m_currentDir, getBody()->getVelocity().y));
}

