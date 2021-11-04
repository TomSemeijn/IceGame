#include "CaveLevel.h"

#include <reb/PointLight.h>
#include <reb/SunLight.h>

#include <reb/ParticleEmitter.h>

#include <reb/PixelParticle.h>
#include <reb/objParticle.h>
#include <reb/Particles/MoveDirShift.h>
#include <reb/Particles/TimedFade.h>
#include <reb/Particles/LightColorSync.h>
#include <reb/Particles/SpriteColorSync.h>
#include <reb/Particles/ColorFade.h>
#include <reb/Particles/Fade.h>
#include <reb/Particles/MoveLine.h>

#include <reb/ShadedSprite.h>
#include <reb/LambdaComp.h>

#include <reb/Concave.h>

#include "ContentIndex.h"
#include "TileIndex.h"

#include "Ghost.h"
#include "GhostAnimator.h"
#include "Iceblock.h"
#include "Camera.h"
#include "LightPulseColor.h"
#include "LightPulseRadius.h"
#include "MultiLiquidEmitter.h"
#include "LambdaTrigger.h"
#include "MoveFromPoint.h"
#include "AccelerateTo.h"

#include "GeneralFactory.h"


#include <reb/Circle.h>
using namespace reb;

extern sf::Font mainFont;

//loads content for the level
void CaveLevel::loadContent(reb::ContentLoader& content) {
	content.loadContentBatch({
		{ContentIndex::GHOST, "Textures/ghost"},
		{ContentIndex::GHOST_PUSH_0, "Textures/ghost_push_0"},
		{ContentIndex::GHOST_PUSH_1, "Textures/ghost_push_1"},
		{ContentIndex::GHOST_BLINK, "Textures/ghost_blink"},
		{ContentIndex::ICEBLOCK, "Textures/iceblock"},
		{TileIndex::CAVEBACKGROUND_0, "Textures/Cave_backgr_0"},
		{ContentIndex::BLOB, "Textures/blob"},
		{ContentIndex::ROCKWALL_0, "Textures/rockWall_0"},
		{ContentIndex::TNT, "Textures/tnt"},
		{ContentIndex::MULTIPLE_TNT_0, "Textures/multipleTnt"},
		{ContentIndex::MULTIPLE_TNT_1, "Textures/multipleTnt1"},
		{ContentIndex::BUTTON, "Textures/button"},
		{ContentIndex::BUTTON_PRESSED, "Textures/button_pressed"},
		{ContentIndex::ROCKWALL_BROKEN_0, "Textures/rockWall_broken_0"},
		{ContentIndex::ROCKWALL_BROKEN_1, "Textures/rockWall_broken_1"},
		{ContentIndex::ROCKWALL_BROKEN_2, "Textures/rockWall_broken_2"},
		{ContentIndex::ROCKWALL_BROKEN_3, "Textures/rockWall_broken_3"},
		{ContentIndex::ROCKWALL_BROKEN_4, "Textures/rockWall_broken_4"},
		{ContentIndex::ROCKWALL_BROKEN_5, "Textures/rockWall_broken_5"},
		{ContentIndex::ROCKWALL_BROKEN_6, "Textures/rockWall_broken_6"},
		{ContentIndex::ROCKWALL_BROKEN_7, "Textures/rockWall_broken_7"},
		{ContentIndex::ROCKWALL_BROKEN_8, "Textures/rockWall_broken_8"},
		{ContentIndex::ROCKWALL_BROKEN_9, "Textures/rockWall_broken_9"},
	});


	//loads the tilemap
	auto& terrainMap = content.loadTilemap(
		"Tilemaps/Level1/cave/cave_tilemap",
		{
			{TileIndex::CAVETILE_1, TileIndex::CAVETILE_45},
			{TileIndex::TORCH, TileIndex::TORCH},
			{TileIndex::GRASSTILE_1, TileIndex::GRASSTILE_12},
			{TileIndex::VINETILE_1, TileIndex::VINETILE_13}
		}
	);

	//specifies a prototype for the torch tile
	terrainMap.specifyPrototype(TileIndex::TORCH, getTorchPrototype(terrainMap.getContent(TORCH), this));

	//sets terrainmap pointer
	m_terrainMap = &terrainMap;
}

//registers the lights for the level
void CaveLevel::registerLights(reb::Shader& shader) {
	shader.addLightType<PointLight>(40);
	shader.addLightType<SunLight>(1);
}

//sets up the level
void CaveLevel::setupLevel(IceGame& game, sf::RenderTarget& window, reb::Scene& scene, reb::ContentLoader& content) {
	//sets up main material
	Material mat{ 1, 0, 1, 0.9 };

	const Rect<float> SCENEBOUNDS{ 0, 0, 1664, 1024 };
	addPlayerBounds(scene, SCENEBOUNDS);

	//adds firefly emitter
	addFireflyEmitter(scene, content.getContent(BLOB), 30, { 50, 150 }, { Vector2(), Vector2(SCENEBOUNDS.getRight(), SCENEBOUNDS.getBottom()) });

	//adds the lightsource from outside the cave
	{
		GameObj* outsideLight = scene.addGameObj(new GameObj(new Transform(false, false, false, Vector2(48, -10))));
		outsideLight->setLight(new PointLight(Color{ 255, 255, 200 }, 150, 200));
	}

	//adds the winning trigger
	{
		auto winningTrigger = scene.addGameObj(new LambdaTrigger(
			[this](LambdaTrigger* trigger, Container* other) {
				if (other == m_iceblock) {
					m_winView->activate();
				}
			},
			[](LambdaTrigger* trigger, Container* other) {},
			new Transform(false, false, false, Vector2(1632, 640))
		));
		winningTrigger->setBody(new Rigidbody(new Convex(Rect<float>{0, 0, 32, 224}), Material{ 0, 0, 0, 0 }, 0, false));
	}

	//adds losing trigger for when the iceblock falls out of bounds
	{
		auto losingTrigger = scene.addGameObj(new LambdaTrigger(
			[this](LambdaTrigger* trigger, Container* other) {
				if (other == m_iceblock) {
					m_loseView->activate();
				}
			},
			[](LambdaTrigger* trigger, Container* other) {},
			new Transform(false, false, false, Vector2(805, 1100))
			));
		losingTrigger->setBody(new Rigidbody(new Convex(Rect<float>{0, 0, 54, 16}), Material{ 0, 0, 0, 0 }, 0, false));
	}

	//sets up the player and the camera
	//Vector2(32, 48)
	Ghost* ghost = (Ghost*)scene.addGameObj(new Ghost(scene, content, Vector2(32, 48), window, SCENEBOUNDS));

	//adds the sunlight
	{
		GameObj* sun = scene.addGameObj(new GameObj());
		sun->setLight(new SunLight(Color(255, 255, 255), 40, Vector2(-2, 1)));
		sun->getTransform()->setZ(5);
	}

	//adds a water emitter for waterfall
	if (true) {
		GameObj* water = scene.addGameObj(new GameObj(new Transform(false, false, false, Vector2(0, 0))));
		water->setLayer(WATER);
		water->setEmitter(
			new MultiLiquidEmitter(
				{
					MultiLiquidEmitter::SpawningArea{{Vector2(1232, 148), Vector2(1296, 164)}, {50, 100}, 300 },
					MultiLiquidEmitter::SpawningArea{{Vector2(544, 256), Vector2(640, 288)}, {3, 15}, 130 },
				},
				Color(50, 100, 255, 180),
				430,
				16,
				SCENEBOUNDS,
				4.5,
				Material(10, 1.175, 0, 0),
				100,
				LayerIndex::WATER,
				5
			)
		);
	}

	//adds the rock walls
	{
		//creates a prototype for all the explosion particle prototypes
		auto getExplosionParticle = [](Vector2 moveFrom) {
			PixelParticle* explosionParticle = new PixelParticle(Color(100, 100, 100));
			explosionParticle->addComp(new Particles::ColorFade({ Color(200, 200, 0), Color(180, 100, 0), Color(100, 100, 100), Color(25, 25, 25), Color(0, 0, 0) }));
			explosionParticle->addComp(new MoveFromPoint(moveFrom, { 50, 60 }));
			explosionParticle->addComp(new Particles::TimedFade({ {0, 1}, {90, 1}, {100, 0} }));
			explosionParticle->addComp(new AccelerateTo(Vector2(0, 1), { 10, 20 }, { 10, 20 }));
			return explosionParticle;
		};
		

		//adds the first rock wall
		{
			//adds the wall
			GameObj* rockWall = scene.addGameObj(new GameObj(new Transform(false, false, false, Vector2(27, 361))));
			rockWall->setLayer(ABOVETERRAIN);
			auto spr = new ShadedSprite(content.getContent(ROCKWALL_0));
			spr->setAlpha(1);
			rockWall->setSprite(spr);
			rockWall->setBody(new Rigidbody(new Concave(content.getContent(ROCKWALL_0)), Material(1000, 0, 1, 0.9), 0, true));

			//adds the tnt
			GameObj* tnt = scene.addGameObj(
				new GameObj(

					new Transform(false, false, false, Vector2(81, 435), 1, getRadians(-40))
				)
			);
			tnt->setLayer(PENGUINLAYER);
			tnt->setSprite(new ShadedSprite(content.getContent(MULTIPLE_TNT_0)));

			//adds the detonator button
			GameObj* button = scene.addGameObj(new LambdaTrigger(
				[rockWall, tnt, &scene, &content](LambdaTrigger* trigger, Container* other) {
				//when the iceblock hits the trigger
				if (other->getLayer() == LayerIndex::ICELAYER) {

					//changes the sprite to the pressed sprite
					trigger->setSprite(new ShadedSprite(content.getContent(BUTTON_PRESSED)));

					//clears the rigidbody of the trigger to prevent this function from being called twice
					trigger->clearBody();

					//destroys the rock wall
					rockWall->destroy();

					//destroys the tnt
					tnt->destroy();

					//spawns smoke particles by enabling, updating and disabling the emitter on the trigger
					trigger->getEmitter()->enable();
					trigger->getEmitter()->update(0);
					trigger->getEmitter()->disable();

					//spawns rocks where the wall was
					for (ContentIndex rockIndex = ROCKWALL_BROKEN_0; rockIndex <= ROCKWALL_BROKEN_9; rockIndex = (ContentIndex)((int)rockIndex + 1)) {
						GameObj* rock = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(genRandf(28, 84), genRandf(375, 442)), 1, genRandf(0, 2.0f * PI))));
						auto rockSpr = new ShadedSprite(content.getContent(rockIndex));
rockSpr->setAlpha(1);
rock->setSprite(rockSpr);
rock->setBody(new Rigidbody(new Concave(content.getContent(rockIndex)), Material(10, 0, 1, 0.9), 100, true));
					}

				}
			},
			[](LambdaTrigger* trigger, Container* other) {},
				new Transform(false, false, false, Vector2(150, 477)))
				);
				button->setSprite(new ShadedSprite(content.getContent(BUTTON)));
				button->setBody(new Rigidbody(new Convex(Rect<float>(0, 2, 5, 19)), Material(0, 0, 0, 0), 0, false));
				button->getTransform()->setFlipX(true);
				button->setLayer(LayerIndex::ABOVETERRAIN);

				//adds the smoke emitter to the detonator button
				auto prototype = getExplosionParticle(Vector2(97, 411));

				button->setEmitter(new ParticleEmitter(
					prototype, 3000, 3000, { 1, 5 },
					{ button->getTransform()->getLocalSpace(Vector2(97, 388)),
						button->getTransform()->getLocalSpace(Vector2(125, 419))
					}, { 1, 3 }, { 0, 2.0f * PI })
				);
				button->getEmitter()->disable();
		}

		//adds the last rock wall
		{
		//adds the wall
		GameObj* rockWall = scene.addGameObj(new GameObj(new Transform(false, false, false, Vector2(1438, 675), 1.6, reb::getRadians(51.5))));
		rockWall->setLayer(ABOVETERRAIN);
		auto spr = new ShadedSprite(content.getContent(ROCKWALL_0));
		spr->setAlpha(1);
		rockWall->setSprite(spr);
		rockWall->setBody(new Rigidbody(new Concave(content.getContent(ROCKWALL_0)), Material(1000, 0, 1, 0.9), 0, true));

		//adds the freely moving tnt block
		GameObj* tnt = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(1017, 811))));
		tnt->setLayer(ABOVETERRAIN);
		tnt->setSprite(new ShadedSprite(content.getContent(TNT)));
		tnt->setBody(new Rigidbody(new Concave(content.getContent(TNT)), Material(5, 0.7, 1, 0.9), 70, true));

		//adds the trigger that sets off the tnt
		GameObj* trigger = scene.addGameObj(new LambdaTrigger(

			//called when something enters the trigger
			[tnt, rockWall, getExplosionParticle, &scene, &content](LambdaTrigger* trigger, Container* other) {

			//if the tnt block enters the trigger
			if (other == tnt) {

				//clears the rigidbody of the trigger to prevent this from being called again
				trigger->clearBody();

				//destroys the rock wall
				rockWall->destroy();

				//clears the sprite and rigidbody of the tnt to "destroy it"
				tnt->clearSprite();
				tnt->clearBody();

				//adds a particle emitter to the tnt
				Vector2 tntCenter = tnt->getTransform()->getWorldSpace(tnt->getTransform()->getCenter());
				tnt->setEmitter(
					new ParticleEmitter(
						getExplosionParticle(Vector2(tntCenter)),
						3000, 3000, { 1, 5 },
						{ tnt->getTransform()->getLocalSpace(tntCenter - Vector2(16, 16)),
							tnt->getTransform()->getLocalSpace(tntCenter + Vector2(-2, 8))
						}, { 1, 3 }, { 0, 2.0f * PI }
					)
				);
				//updates the emitter to spawn all the particles
				tnt->getEmitter()->update(0);

				//disables the emitter to prevent more particles from spawning
				tnt->getEmitter()->disable();

				//spawns rocks where the wall was
				for (ContentIndex rockIndex = ROCKWALL_BROKEN_0; rockIndex <= ROCKWALL_BROKEN_9; rockIndex = (ContentIndex)((int)rockIndex + 1)) {
					GameObj* rock = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(genRandf(1464, 1569), genRandf(697, 787)), 1.6, genRandf(0, 2.0f * PI))));
					auto rockSpr = new ShadedSprite(content.getContent(rockIndex));
					rockSpr->setAlpha(1);
					rock->setSprite(rockSpr);
					rock->setBody(new Rigidbody(new Concave(content.getContent(rockIndex)), Material(10, 0, 1, 0.9), 100, true));
				}

			}

		},
			[](LambdaTrigger* trigger, Container* other) {},
			new Transform(false, false, false, Vector2(1436, 675), 1.6, reb::getRadians(51.5))
			));
		trigger->setBody(new Rigidbody(new Concave(content.getContent(ROCKWALL_0)), Material(0, 0, 0, 0), 0, false));

		}
	}

	//adds some rocks
	for (ContentIndex rockIndex = ROCKWALL_BROKEN_0; rockIndex <= ROCKWALL_BROKEN_9; rockIndex = (ContentIndex)((int)rockIndex + 1)) {
		GameObj* rock = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(genRandf(378, 711), genRandf(717, 804)), 1.3, genRandf(0, 2.0f * PI))));
		auto rockSpr = new ShadedSprite(content.getContent(rockIndex));
		rockSpr->setAlpha(1);
		rock->setSprite(rockSpr);
		rock->setBody(new Rigidbody(new Concave(content.getContent(rockIndex)), Material(10, 0, 1, 0.9), 100, true));
	}

	//adds the iceblock
	//Vector2(567, 102)
	m_iceblock = (Iceblock*)scene.addGameObj(new Iceblock(this, content, Vector2(567, 102)));

	//adds the background tilemap
	{
		//adds set
		Tileset& backgSet = scene.addTileset(32, 32, LayerIndex::BACKGROUND);
		//registers background tile
		{
			auto backgroundSpr = new ShadedSprite(content.getContent(TileIndex::CAVEBACKGROUND_0));
			auto backgTile = new Tile();
			backgTile->setSprite(backgroundSpr);
			backgSet.registerTile(TileIndex::CAVEBACKGROUND_0, backgTile);
		}
		//sets tiles
		backgSet.setTiles({ 0, 51 }, { 0, 31 }, TileIndex::CAVEBACKGROUND_0);
	}
	
	//adds the terrain map to the terrain layer
	m_terrainMap->addMap(&scene, "caveTerrain", LayerIndex::TERRAIN, Material{ 1, 0, 1, 0.8 });

	//adds the torch map to the torch layer
	m_terrainMap->addMap(&scene, "caveTorches", LayerIndex::TORCHES);

	//adds the vine map to the middleground layer
	m_terrainMap->addMap(&scene, "caveVines", LayerIndex::MIDDLEGROUND);

	//adds the grass map for the terrainTop layer
	m_terrainMap->addMap(&scene, "CaveGrass", LayerIndex::TERRAINTOP);

	scene.getTileset(LayerIndex::TORCHES).m_hasCollision = false;
	scene.getTileset(LayerIndex::MIDDLEGROUND).m_hasCollision = false;
	scene.getTileset(LayerIndex::BACKGROUND).m_hasCollision = false;
	scene.getTileset(LayerIndex::TERRAINTOP).m_hasCollision = false;

	scene.getTileset(LayerIndex::TERRAIN).m_isUpdated = false;
	scene.getTileset(LayerIndex::MIDDLEGROUND).m_isUpdated = false;
	scene.getTileset(LayerIndex::BACKGROUND).m_isUpdated = false;
	scene.getTileset(LayerIndex::TERRAINTOP).m_isUpdated = false;

}

//sets up the gui
void CaveLevel::setupGUI(IceGame& game, sf::RenderTarget& window, reb::GUI& gui, reb::ContentLoader& content) {
	setupLevelMenu(game, gui, content);
	setupIceMeter(m_iceblock, 200, gui, content);
	setupWinView(game, gui, content, LEVEL_LAVA);

	/*auto fpsView = gui.addView(new View(Vector2(1000, 1000)));
	
	class Fps : public Views::TextComp {
	private:
		IceGame* m_game = nullptr;
	public:
		Fps(IceGame* game) : m_game(game), Views::TextComp(mainFont, 24) {};

		virtual void update(float elapsed)override {
			setText(std::to_string(m_game->getFps()));
		};
	};

	auto fpsText = new Fps(&game);

	fpsView->addComp(fpsText);*/
}