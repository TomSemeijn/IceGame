#include "IceGame.h"

#include <reb/PointLight.h>
#include <reb/SunLight.h>

#include <reb/ShadedSprite.h>

#include <reb/ParticleEmitter.h>
#include <reb/PixelParticle.h>
#include <reb/Particles/ColorFade.h>
#include <reb/Particles/Fade.h>
#include <reb/Particles/MoveLine.h>
#include <reb/objParticle.h>
#include <reb/Particles/TimedFade.h>
#include <reb/Particles/LightColorSync.h>
#include <reb/Particles/SpriteColorSync.h>
#include <reb/Particles/MoveDirShift.h>

#include <reb/Views/TextComp.h>
#include <reb/Views/SpriteComp.h>

#include "ContentIndex.h"
#include "TileIndex.h"
#include "LayerIndex.h"

#include "LightPulseColor.h"
#include "LightPulseRadius.h"

#include "GeneralFactory.h"

using namespace reb;

extern sf::Font mainFont;

//sets up the loading screen
Scene* IceGame::setupLoadingScreen(ContentLoader& newLoader, GUI& newGui, Shader& newShader) {
	Scene* loadingScreen = new Scene();

	//registers lighttypes
	newShader.addLightType<PointLight>(13);
	newShader.addLightType<SunLight>(1);

	//gets content for loading screen
	newLoader.loadContentBatch(
		{
			{GAME_TITLE, "Textures/GUI/Title"},
			{CAVEBACKGROUND_0, "Textures/Cave_backgr_0"},
			{TORCH, "Textures/torch"},
			{ContentIndex::BLOB, "Textures/blob"},
			{ContentIndex::TEXTBOX, "Textures/GUI/textbox"}
		}
	);
	newLoader.manage();

	//adds loading textbox
	View* textView = new View(Vector2(500, 80), nullptr, Vector2(470, 600));
	//textView->addComp(new Views::RectComp(Color(150, 150, 150, 200), Color(60, 60, 60), 6));
	textView->addComp(new Views::SpriteComp(newLoader.getContent(ContentIndex::TEXTBOX)));
	m_setupTextComp = new Views::WriteTextComp(30, mainFont, 24, true, Color(0, 0, 0, 200), "Loading...");
	m_setupTextComp->setPaddingLeft(10);
	m_setupTextComp->setPaddingRight(10);
	m_setupTextComp->setPaddingTop(6);
	m_setupTextComp->setPaddingBottom(6);
	textView->addComp(m_setupTextComp);
	newGui.addView(textView);

	//sets the background
	Tileset& backgroundSet = loadingScreen->addTileset(32, 32, BACKGROUND);
	auto tile = new Tile();
	tile->setSprite(new ShadedSprite(newLoader.getContent(CAVEBACKGROUND_0)));
	backgroundSet.registerTile(CAVEBACKGROUND_0, tile);
	backgroundSet.setTiles({ 0, 15 }, { 0, 9 }, CAVEBACKGROUND_0);

	//adds a sunlight
	{
		GameObj* sun = loadingScreen->addGameObj(new GameObj());
		sun->getTransform()->setZ(5);
		sun->setLight(new SunLight(Color(255, 255, 255), 30, Vector2(-2, 1)));
	}

	//sets some torches
	Tileset& torchSet = loadingScreen->addTileset(32, 32, TORCHES);
	//registers the prototype for the torch tile
	torchSet.registerTile(TORCH, getTorchPrototype(newLoader.getContent(TORCH)));

	//sets some torches
	torchSet.setTile(2, 4, TORCH);
	torchSet.setTile(12, 4, TORCH);
	torchSet.setTile(7, 2, TORCH);

	//adds the game title
	GameObj* gameTitle = loadingScreen->addGameObj(new GameObj(new Transform(true, false, false, Vector2(), 0.5), Container::BOUNDS_SPRITE));
	gameTitle->setSprite(new ShadedSprite(newLoader.getContent(GAME_TITLE)));
	gameTitle->setLayer(LayerIndex::FOREGROUND);
	((ShadedSprite*)gameTitle->getSprite())->setAlpha(5);
	gameTitle->getTransform()->setZ(-5);
	Vector2 worldCenter = m_window.getView().getSize();
	worldCenter /= 2.0f;
	Vector2 titleWorldCenter = gameTitle->getTransform()->getWorldSpace(gameTitle->getTransform()->getBounds().getCenter());
	gameTitle->getTransform()->setPos(worldCenter - titleWorldCenter);

	//adds an object particle emitter
	addFireflyEmitter(*loadingScreen, newLoader.getContent(BLOB), 10, { 5, 15 }, { Vector2(50, 50), Vector2(430, 220) });

	return loadingScreen;
}
