#include "IceLevel.h"

#include <array>

#include <reb/ShadedSprite.h>
#include <reb/Concave.h>
#include <reb/SunLight.h>
#include <reb/PointLight.h>
#include <reb/LineLight.h>
#include <reb/LambdaComp.h>

#include <reb/ParticleEmitter.h>
#include <reb/objParticle.h>
#include <reb/Particles/TimedFade.h>
#include <reb/Particles/MoveLine.h>
#include <reb/Particles/TimedScale.h>

#include "Ghost.h"
#include "GhostAnimator.h"
#include "Camera.h"
#include "RandSpriteParticle.h"
#include "HorizontalShake.h"
#include "RotationalShake.h"
#include "Penguin.h"
#include "Scale.h"
#include "Gate.h"
#include "OpacityScaleSync.h"
#include "RandSpriteParticleObject.h"
#include "ObjScaleSync.h"
#include "LambdaTrigger.h"

#include "TileIndex.h"
#include "ContentIndex.h"
#include "LayerIndex.h"

using namespace reb;

void IceLevel::loadContent(reb::ContentLoader& content) {
	content.loadContentBatch({
		{ContentIndex::GHOST, "Textures/ghost"},
		{ContentIndex::GHOST_PUSH_0, "Textures/ghost_push_0"},
		{ContentIndex::GHOST_PUSH_1, "Textures/ghost_push_1"},
		{ContentIndex::GHOST_BLINK, "Textures/ghost_blink"},
		{ContentIndex::ICEBLOCK, "Textures/iceblock"},
		{ContentIndex::SNOWMAN, "Textures/icelevel/snowman"},
		{ContentIndex::SNOWBALL, "Textures/icelevel/snowball"},
		{ContentIndex::SNOWFLAKE_0, "Textures/icelevel/snowflake_0"},
		{ContentIndex::SNOWFLAKE_1, "Textures/icelevel/snowflake_1"},
		{ContentIndex::SNOWFLAKE_2, "Textures/icelevel/snowflake_2"},
		{ContentIndex::PENGUIN_0, "Textures/icelevel/penguin_0"},
		{ContentIndex::PENGUIN_1, "Textures/icelevel/penguin_1"},
		{ContentIndex::PENGUIN_2, "Textures/icelevel/penguin_2"},
		{ContentIndex::FALLING_ICICLE_0, "Textures/icelevel/falling_icicle_0"},
		{ContentIndex::FALLING_ICICLE_1, "Textures/icelevel/falling_icicle_1"},
		{ContentIndex::FALLING_ICICLE_2, "Textures/icelevel/falling_icicle_2"},
		{ContentIndex::ICICLEWALL, "Textures/icelevel/iciclewall"},
		{ContentIndex::CRYSTAL_1, "Textures/icelevel/crystal_1"},
		{ContentIndex::GATE, "Textures/icelevel/gate"},
		{ContentIndex::SCALE, "Textures/icelevel/scale (top-only geometry)"},
	});

	//loads the tilemap
	auto& terrainMap = content.loadTilemap(
		"Tilemaps/Level3/ice/ice_tilemap",
		{
			{TileIndex::ICEDIRT_1, TileIndex::ICEDIRT_12},
			{TileIndex::DIRT_1, TileIndex::DIRT_13},
			{TileIndex::ICICLE_1, TileIndex::ICICLE_3},
			{TileIndex::ICEBACKGROUND, TileIndex::ICEBACKGROUND}
		}
	);

	//specifies prototypes for ice tiles to add alpha value
	for (TileIndex tile = TileIndex::ICEDIRT_1; tile <= TileIndex::ICEDIRT_12; tile = (TileIndex)((int)tile + 1)) {
		ShadedSprite* spr = new ShadedSprite(terrainMap.getContent(tile));
		spr->setAlpha(4);
		Tile* prototype = new Tile();
		prototype->setSprite(spr);
		prototype->setBody(new Rigidbody(new Concave(terrainMap.getContent(tile)), Material(1, 0.1, 0, 0), 0, true));
		terrainMap.specifyPrototype(tile, prototype);
	}

	//specifies prototypes for icicles to add alpha value and particles
	for (TileIndex tile = TileIndex::ICICLE_1; tile <= TileIndex::ICICLE_3; tile = (TileIndex)((int)tile + 1)) {
		
		//sets up particle object prototype
		RandSpriteParticleObject* protoObj = new RandSpriteParticleObject({
			content.getContent(FALLING_ICICLE_0),
			content.getContent(FALLING_ICICLE_1),
			content.getContent(FALLING_ICICLE_2),
		}, new Transform(true, true));
		auto protoSpr = new ShadedSprite(content.getContent(FALLING_ICICLE_0));
		protoSpr->setAlpha(4);
		protoObj->setSprite(protoSpr);
		protoObj->setBody(new Rigidbody(new Concave(content.getContent(FALLING_ICICLE_0)), Material(150, 0, 0, 0), 100, true));
		protoObj->setLayer(LayerIndex::ICELAYER);

		//sets up particle prototype
		ObjParticle* protoParticle = new ObjParticle(protoObj, false);
		protoParticle->addComp(new Particles::TimedScale({ { 5, 2 }, { 95, 2 }, { 100, 0 } }));
		protoParticle->addComp(new ObjScaleSync());

		//sets up emitter
		ParticleEmitter* emitter = new ParticleEmitter(
			protoParticle, 
			2, 5, 
			{ 4, 8 }, 
			{ Vector2(0, 32), Vector2(32, 36) }, 
			{ 1, 1.5 }, 
			{ 0, 0 }
		);
		
		//sets up sprite
		ShadedSprite* spr = new ShadedSprite(terrainMap.getContent(tile));
		spr->setAlpha(4);

		//creates and specifies prototype
		Tile* prototype = new Tile();
		prototype->setSprite(spr);
		prototype->setBody(new Rigidbody(
			new Concave(terrainMap.getContent(tile)),
			Material(1, 0.1, 0, 0),
			0,
			true
		));
		prototype->setEmitter(emitter);
		terrainMap.specifyPrototype(tile, prototype);
	}
	
	//sets terrainmap pointer
	m_terrainMap = &terrainMap;
}

void IceLevel::setupLevel(IceGame& game, sf::RenderTarget& window, reb::Scene& scene, reb::ContentLoader& content) {

	//sets up main material
	Material mat{ 1, 0, 1, 0.9 };

	const Rect<float> SCENEBOUNDS{ 0, 0, 1920, 2272 };
	addPlayerBounds(scene, SCENEBOUNDS);

	//adds the iceblock
	m_iceblock = (Iceblock*)scene.addGameObj(new Iceblock(this, content, Vector2(664, 88)));

	//sets up the player and the camera
	Ghost* ghost = (Ghost*)scene.addGameObj(new Ghost(scene, content, Vector2(332, 64), window, SCENEBOUNDS));

	//adds the winning trigger
	{
		auto winningTrigger = scene.addGameObj(new LambdaTrigger(
			[this](LambdaTrigger* trigger, Container* other) {
			if (other == m_iceblock) {
				m_winView->activate();
			}
		},
			[](LambdaTrigger* trigger, Container* other) {},
			new Transform(false, false, false, Vector2(-20, 1827))
			));
		winningTrigger->setBody(new Rigidbody(new Convex(Rect<float>{0, 0, 38, 233}), Material{ 0, 0, 0, 0 }, 0, false));
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
			new Transform(false, false, false, Vector2(1930, 1419))
			));
		losingTrigger->setBody(new Rigidbody(new Convex(Rect<float>{0, 0, 5, 122}), Material{ 0, 0, 0, 0 }, 0, false));
	}

	//adds the sunlight
	{
		GameObj* sun = scene.addGameObj(new GameObj());
		sun->setLight(new SunLight(Color(255, 255, 255), 40, Vector2(-2, 1)));
		sun->getTransform()->setZ(5);
	}

	//adds snowballs
	{
		Material snowballMat{ 4, 0.8, 0.4, 0.1 };
		float snowballGravity = 80;
		{
			GameObj* snowBall = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(1360, 856))));
			snowBall->setBody(new Rigidbody(new Concave(content.getContent(ContentIndex::SNOWBALL)), snowballMat, snowballGravity, true));
			auto spr = new ShadedSprite(content.getContent(ContentIndex::SNOWBALL));
			spr->setAlpha(5);
			snowBall->setSprite(spr);
			snowBall->setLayer(LayerIndex::ICELAYER);
		}
		{
			GameObj* snowBall = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(384, 1248))));
			snowBall->setBody(new Rigidbody(new Concave(content.getContent(ContentIndex::SNOWBALL)), snowballMat, snowballGravity, true));
			auto spr = new ShadedSprite(content.getContent(ContentIndex::SNOWBALL));
			spr->setAlpha(5);
			snowBall->setSprite(spr);
			snowBall->setLayer(LayerIndex::ICELAYER);
		}
	}

	//adds snowflake particles
	{
		GameObj* emitter = scene.addGameObj(new GameObj());
		RandSpriteParticle* snowflake = new RandSpriteParticle(
			{
				content.getContent(SNOWFLAKE_0), 
				content.getContent(SNOWFLAKE_1), 
				content.getContent(SNOWFLAKE_2)
			}
		);
		snowflake->addComp(new Particles::TimedFade({ { 5, 0.8 }, { 95, 0.8 }, { 100, 0 } }));
		snowflake->addComp(new Particles::MoveLine({ 60, 75 }, {10, 20}));
		snowflake->addComp(new HorizontalShake({ 2, 5 }, {2, 3}));
		snowflake->addComp(new RotationalShake({0.25, 0.5}, {1, 4}));
		emitter->setEmitter(new ParticleEmitter(snowflake, 1000, 200, { 40, 70 }, { Vector2(-500, -800), Vector2(SCENEBOUNDS.getRight(), SCENEBOUNDS.getBottom()) }, { 0.9, 1.1 }, { -0.1, 0.1 }));
		emitter->setLayer(LayerIndex::FIREFLYLAYER);
	}

	//adds a snowman just for fun
	{
		GameObj* snowman = scene.addGameObj(new GameObj(new Transform(false, false, false, Vector2(224, 2000))));
		snowman->setSprite(new ShadedSprite(content.getContent(ContentIndex::SNOWMAN)));
		snowman->getTransform()->setFlipX(true);
	}

	//adds penguins
	{
		scene.addGameObj(new Penguin(this, scene, content, Vector2(905, 170)));
		scene.addGameObj(new Penguin(this, scene, content, Vector2(1123, 170)));

		scene.addGameObj(new Penguin(this, scene, content, Vector2(747, 730)));
		scene.addGameObj(new Penguin(this, scene, content, Vector2(861, 730)));
		scene.addGameObj(new Penguin(this, scene, content, Vector2(934, 730)));

		scene.addGameObj(new Penguin(this, scene, content, Vector2(1280, 1315)));
		scene.addGameObj(new Penguin(this, scene, content, Vector2(1377, 1315)));
		scene.addGameObj(new Penguin(this, scene, content, Vector2(1475, 1315)));
		scene.addGameObj(new Penguin(this, scene, content, Vector2(1568, 1315)));

		scene.addGameObj(new Penguin(this, scene, content, Vector2(350, 2020)));
		scene.addGameObj(new Penguin(this, scene, content, Vector2(448, 2020)));
		scene.addGameObj(new Penguin(this, scene, content, Vector2(547, 2020)));
	}

	//adds icicle walls
	{
		{
			GameObj* test = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(297, 70), 0.6, reb::getRadians(-90))));
			auto spr = new ShadedSprite(content.getContent(ICICLEWALL));
			spr->setAlpha(5);
			test->setSprite(spr);
			test->setBody(new Rigidbody(new Concave(content.getContent(ICICLEWALL)), Material(10, 0, 0, 0), 100, true));
			test->setLayer(ICELAYER);
		}
		
		{
			GameObj* test = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(1274, 275))));
			auto spr = new ShadedSprite(content.getContent(ICICLEWALL));
			spr->setAlpha(5);
			test->setSprite(spr);
			test->setBody(new Rigidbody(new Concave(content.getContent(ICICLEWALL)), Material(10, 0, 0, 0), 100, true));
			test->setLayer(ICELAYER);
		}
		{
			GameObj* test = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(1306, 1036))));
			test->getTransform()->setFlipX(true);
			auto spr = new ShadedSprite(content.getContent(ICICLEWALL));
			spr->setAlpha(5);
			test->setSprite(spr);
			test->setBody(new Rigidbody(new Concave(content.getContent(ICICLEWALL)), Material(10, 0, 0, 0), 100, true));
			test->setLayer(ICELAYER);
		}
		{
			GameObj* test = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(988, 495))));
			test->getTransform()->setFlipX(true);
			auto spr = new ShadedSprite(content.getContent(ICICLEWALL));
			spr->setAlpha(5);
			test->setSprite(spr);
			test->setBody(new Rigidbody(new Concave(content.getContent(ICICLEWALL)), Material(10, 0, 0, 0), 100, true));
			test->setLayer(ICELAYER);
		}
		{
			GameObj* test = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(1628, 1452))));
			auto spr = new ShadedSprite(content.getContent(ICICLEWALL));
			spr->setAlpha(5);
			test->setSprite(spr);
			test->setBody(new Rigidbody(new Concave(content.getContent(ICICLEWALL)), Material(10, 0, 0, 0), 100, true));
			test->setLayer(ICELAYER);
		}
		{
			GameObj* test = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(935, 2093))));
			auto spr = new ShadedSprite(content.getContent(ICICLEWALL));
			spr->setAlpha(5);
			test->setSprite(spr);
			test->setBody(new Rigidbody(new Concave(content.getContent(ICICLEWALL)), Material(10, 0, 0, 0), 100, true));
			test->setLayer(ICELAYER);
		}

		for(int k = 0; k < 6; k++){
			GameObj* test = scene.addGameObj(new GameObj(new Transform(true, false, false, Vector2(880 + 43 * k, 1900), 1, reb::getRadians(90))));
			auto spr = new ShadedSprite(content.getContent(ICICLEWALL));
			spr->setAlpha(5);
			test->setSprite(spr);
			test->setBody(new Rigidbody(new Concave(content.getContent(ICICLEWALL)), Material(10, 0, 0, 0), 100, true));
			test->setLayer(ICELAYER);
		}
	}

	//adds decorative crystals
	{
		int crystalOpacity = 255;
		std::array<Color, 4> crystalColors{ 
			Color{255, 200, 30, crystalOpacity},
			Color{255, 0, 30, crystalOpacity},
			Color{150, 30, 255, crystalOpacity},
			Color{30, 255, 100, crystalOpacity}
		};
		addCrystal(scene, content, crystalColors[0], Vector2(275, 184), 0);
		addCrystal(scene, content, crystalColors[1], Vector2(303, 12), getRadians(-51));
		addCrystal(scene, content, crystalColors[2], Vector2(517, 175), getRadians(54));
		addCrystal(scene, content, crystalColors[3], Vector2(877, 40), getRadians(180));
		addCrystal(scene, content, crystalColors[1], Vector2(1143, 181), getRadians(-50));
		addCrystal(scene, content, crystalColors[0], Vector2(1339, 38), getRadians(206));
		addCrystal(scene, content, crystalColors[3], Vector2(1352, 194), getRadians(65));
		addCrystal(scene, content, crystalColors[2], Vector2(1467, 342), getRadians(-40), true);
		addCrystal(scene, content, crystalColors[0], Vector2(1467, 487), getRadians(-48));
		addCrystal(scene, content, crystalColors[3], Vector2(1345, 577), getRadians(-223));
		addCrystal(scene, content, crystalColors[1], Vector2(1174, 695), getRadians(-10));
		addCrystal(scene, content, crystalColors[0], Vector2(782, 457), getRadians(-223));
		addCrystal(scene, content, crystalColors[2], Vector2(924, 575), getRadians(-125));
		addCrystal(scene, content, crystalColors[3], Vector2(567, 524), getRadians(-125));
		addCrystal(scene, content, crystalColors[2], Vector2(408, 724), getRadians(-40));
		addCrystal(scene, content, crystalColors[0], Vector2(265, 891), getRadians(90));
		addCrystal(scene, content, crystalColors[1], Vector2(506, 1024), getRadians(-123));
		addCrystal(scene, content, crystalColors[3], Vector2(258, 1178), getRadians(30), true);
		addCrystal(scene, content, crystalColors[2], Vector2(458, 1337), 0);
		addCrystal(scene, content, crystalColors[0], Vector2(759, 1158), getRadians(180));
		addCrystal(scene, content, crystalColors[1], Vector2(1080, 1304), getRadians(-27));
		addCrystal(scene, content, crystalColors[3], Vector2(1343, 1124), getRadians(137));
		addCrystal(scene, content, crystalColors[2], Vector2(1883, 1207), getRadians(-139));
		addCrystal(scene, content, crystalColors[1], Vector2(1672, 1417), getRadians(62));
		addCrystal(scene, content, crystalColors[0], Vector2(1876, 1654), getRadians(-29));
		addCrystal(scene, content, crystalColors[3], Vector2(1610, 1708), getRadians(130));
		addCrystal(scene, content, crystalColors[2], Vector2(1546, 2006), getRadians(-6), true);
		addCrystal(scene, content, crystalColors[1], Vector2(1269, 1839), getRadians(-129));
		addCrystal(scene, content, crystalColors[0], Vector2(1179, 2038), getRadians(-62));
		addCrystal(scene, content, crystalColors[2], Vector2(965, 1859), getRadians(121));
		addCrystal(scene, content, crystalColors[3], Vector2(966, 2173), getRadians(55));
		addCrystal(scene, content, crystalColors[1], Vector2(704, 1862), getRadians(180));
		addCrystal(scene, content, crystalColors[2], Vector2(593, 2008), getRadians(-20));
		addCrystal(scene, content, crystalColors[3], Vector2(408, 1830), getRadians(-149));
		addCrystal(scene, content, crystalColors[0], Vector2(57, 2006), getRadians(15), true);
	}
	
	//adds scales and gates
	{
		scene.addGameObj(new Gate(content, (Scale*)scene.addGameObj(new Scale(content, Vector2(608, 1320))), Vector2(878, 1136)));
		scene.addGameObj(new Gate(content, (Scale*)scene.addGameObj(new Scale(content, Vector2(629, 1992))), Vector2(88, 1839)));
	}

	//adds the background
	m_terrainMap->addMap(&scene, "background", LayerIndex::BACKGROUND);
	scene.getTileset(LayerIndex::BACKGROUND).m_hasCollision = false;
	scene.getTileset(LayerIndex::BACKGROUND).m_isUpdated = false;

	//adds the terrain
	m_terrainMap->addMap(&scene, "dirt", LayerIndex::TERRAIN);
	scene.getTileset(LayerIndex::TERRAIN).m_isUpdated = false;

	//adds the ice
	m_terrainMap->addMap(&scene, "ice", LayerIndex::ICETERRAIN);

}

//adds a crystal with the given color
void IceLevel::addCrystal(reb::Scene& scene, reb::ContentLoader& content, reb::Color color, reb::Vector2 pos, float rotation, bool flipX) {
	GameObj* crystal = scene.addGameObj(new GameObj(new Transform(false, false, false, pos, 1, rotation)));
	if (flipX) {
		crystal->getTransform()->setFlipX(true);
	}
	crystal->setLayer(LayerIndex::TERRAINTOP);
	auto spr = new ShadedSprite(content.getContent(CRYSTAL_1));
	spr->setColor(color);
	spr->setAlpha(5);
	crystal->setSprite(spr);
	crystal->getTransform()->setCenter(Vector2(16, 16));
	crystal->setLight(new LineLight(color, 200, 214, { Vector2(14, 5), Vector2(16.5, 22) }));
	crystal->getLight()->setOffset(Vector3(0, 0, 5));
}

void IceLevel::setupGUI(IceGame& game, sf::RenderTarget& window, reb::GUI& gui, reb::ContentLoader& content) {
	setupLevelMenu(game, gui, content);
	setupIceMeter(m_iceblock, 200, gui, content);
	setupWinView(game, gui, content, NOLEVEL_MAIN);
}

void IceLevel::registerLights(reb::Shader& shader) {
	shader.addLightType<SunLight>(1);
	shader.addLightType<PointLight>(13);
	shader.addLightType<LineLight>(30);
}