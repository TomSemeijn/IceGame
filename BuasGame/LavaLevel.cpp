#include "LavalLevel.h"

#include <reb/ShadedSprite.h>
#include <reb/SunLight.h>
#include <reb/PointLight.h>

#include <reb/Concave.h>

#include <reb/Particles/MoveLine.h>
#include <reb/Particles/TimedFade.h>

#include <reb/LambdaComp.h>

#include "Ghost.h"
#include "GhostAnimator.h"
#include "Camera.h"
#include "HeatTile.h"
#include "RandSpriteParticle.h"
#include "OpacityScaleSync.h"
#include "TriggerHeat.h"
#include "MultiLiquidEmitter.h"
#include "LavaHeat.h"
#include "LavaObject.h"
#include "HeatObject.h"
#include "LiquidGlow.h"
#include "RandSpriteParticleObject.h"
#include "LambdaTrigger.h"
#include "DespawnBounds.h"
#include "DestroyOutBounds.h"
#include "ReversePosSync.h"
#include "PointHeat.h"
#include "LambdaCollide.h"
#include "ObsidianLiquid.h"
#include "HorizontalShake.h"
#include "RotationalShake.h"

#include "GeneralFactory.h"

#include "TileIndex.h"
#include "ContentIndex.h"

using namespace reb;

//loads content for the level
void LavaLevel::loadContent(reb::ContentLoader& content) {
	content.loadContentBatch({
		{ContentIndex::GHOST, "Textures/ghost"},
		{ContentIndex::GHOST_PUSH_0, "Textures/ghost_push_0"},
		{ContentIndex::GHOST_PUSH_1, "Textures/ghost_push_1"},
		{ContentIndex::GHOST_BLINK, "Textures/ghost_blink"},
		{ContentIndex::ICEBLOCK, "Textures/iceblock"},
		{ContentIndex::DROP_0, "Textures/lavalevel/drop_0"},
		{ContentIndex::DROP_1, "Textures/lavalevel/drop_1"},
		{ContentIndex::DROP_2, "Textures/lavalevel/drop_2"},
		{ContentIndex::DROP_3, "Textures/lavalevel/drop_3"},
		{TileIndex::LAVASTONEBACKGROUND, "Textures/lavalevel/background"},
		{ContentIndex::PIPE_TOP, "Textures/lavalevel/pipe_top"},
		{ContentIndex::PIPE_BOTTOM, "Textures/lavalevel/pipe_bottom"},
		{ContentIndex::ROCK_0, "Textures/lavalevel/rock_0"},
		{ContentIndex::ROCK_1, "Textures/lavalevel/rock_1"},
		{ContentIndex::ROCK_2, "Textures/lavalevel/rock_2"},
		{ContentIndex::ROCK_3, "Textures/lavalevel/rock_3"},
		{ContentIndex::BOAT, "Textures/lavalevel/boat"},
		{ContentIndex::BRIDGEPIECE_0, "Textures/lavalevel/bridgePiece_0"},
		{ContentIndex::BRIDGEPIECE_1, "Textures/lavalevel/bridgePiece_1"},
		{ContentIndex::BRIDGEPIECE_2, "Textures/lavalevel/bridgePiece_2"},
		{ContentIndex::BRIDGEPIECE_3, "Textures/lavalevel/bridgePiece_3"},
		{ContentIndex::BRIDGEPIECE_4, "Textures/lavalevel/bridgePiece_4"},
		{ContentIndex::BRIDGEPIECE_5, "Textures/lavalevel/bridgePiece_5"},
		{ContentIndex::BRIDGEPIECE_6, "Textures/lavalevel/bridgePiece_6"},
		{ContentIndex::BRIDGEPIECE_7, "Textures/lavalevel/bridgePiece_7"},
		{ContentIndex::BRIDGEPIECE_8, "Textures/lavalevel/bridgePiece_8"},
		{ContentIndex::BRIDGEPIECE_9, "Textures/lavalevel/bridgePiece_9"},
		{ContentIndex::BRIDGE, "Textures/lavalevel/bridge"},
		{ContentIndex::BOATPIECE_0, "Textures/lavalevel/boatPiece_0"},
		{ContentIndex::BOATPIECE_1, "Textures/lavalevel/boatPiece_1"},
		{ContentIndex::BOATPIECE_2, "Textures/lavalevel/boatPiece_2"},
		{ContentIndex::BOATPIECE_3, "Textures/lavalevel/boatPiece_3"},
		{ContentIndex::BOATPIECE_4, "Textures/lavalevel/boatPiece_4"},
		{ContentIndex::BOATPIECE_5, "Textures/lavalevel/boatPiece_5"},
		{ContentIndex::BUTTON, "Textures/button"},
		{ContentIndex::BUTTON_PRESSED, "Textures/button_pressed"},
		{ContentIndex::OBSIDIAN, "Textures/lavalevel/obsidian"},
		{ContentIndex::REGENSTATION, "Textures/lavalevel/regenStation"},
		{ContentIndex::SNOWFLAKE_0, "Textures/icelevel/snowflake_0"},
		{ContentIndex::SNOWFLAKE_1, "Textures/icelevel/snowflake_1"},
		{ContentIndex::SNOWFLAKE_2, "Textures/icelevel/snowflake_2"},
	});

	//loads the tilemap
	auto& terrainMap = content.loadTilemap(
		"Tilemaps/Level2/lava/lava_tilemap",
		{
			{TileIndex::LAVATILE_1, TileIndex::LAVATILE_45},
			{TileIndex::DRIPPYTILE_1, TileIndex::DRIPPYTILE_5},
			{TileIndex::TORCH, TileIndex::TORCH},
		}
	);

	//specifies a prototype for every drippy tile to add particles and heat
	for (TileIndex tile = TileIndex::DRIPPYTILE_1; tile <= TileIndex::DRIPPYTILE_5; tile = (TileIndex)((int)tile + 1)) {
		HeatTile* tilePtr = new HeatTile(this, new TriggerHeat(200));
		tilePtr->setSprite(new ShadedSprite(terrainMap.getContent(tile)));
		((ShadedSprite*)tilePtr->getSprite())->setAlpha(10);
		tilePtr->setBody(new Rigidbody(new Concave(terrainMap.getContent(tile)), Material(1, 0, 1, 0.8), 0, false));
		std::pair<Vector2, Vector2> particleSpawnRect;
		switch (tile) {
		case DRIPPYTILE_1:
			particleSpawnRect = { Vector2(0, 16), Vector2(14, 25) };
			break;
		case DRIPPYTILE_2:
			particleSpawnRect = { Vector2(16, 17), Vector2(32, 32) };
			break;
		case DRIPPYTILE_3:
			particleSpawnRect = { Vector2(16, 8), Vector2(31, 15) };
			break;
		case DRIPPYTILE_4:
			particleSpawnRect = { Vector2(0, 10), Vector2(31, 15) };
			break;
		case DRIPPYTILE_5:
			particleSpawnRect = { Vector2(0, 5), Vector2(15, 15) };
			break;
		}
		auto particle = new RandSpriteParticle(
			{
				content.getContent(DROP_0),
				content.getContent(DROP_1),
				content.getContent(DROP_2),
				content.getContent(DROP_3),
			}
		);
		particle->addComp(new Particles::TimedFade({ {30, 0.6}, {40, .6}, {100, 0} }));
		particle->addComp(new OpacityScaleSync());
		particle->addComp(new Particles::MoveLine({ 90, 90 }, { 10, 20 }));
		tilePtr->setEmitter(new ParticleEmitter(particle, 4, 4, { 1, 5 }, particleSpawnRect, { 1, 1 }, { 0, 0 }));
		terrainMap.specifyPrototype(tile, tilePtr);
	}

	//specifies a prototype for every terrain tile to add an alpha value to their sprite
	for (TileIndex tile = TileIndex::LAVATILE_1; tile <= TileIndex::LAVATILE_45; tile = (TileIndex)((int)tile + 1)) {
		auto spr = new ShadedSprite(terrainMap.getContent(tile));
		spr->setAlpha(10);
		auto tileProto = new Tile();
		tileProto->setSprite(spr);
		tileProto->setBody(new Rigidbody(new Concave(terrainMap.getContent(tile)), Material{ 1, 0, 1, 0.8 }, 0, true));
		terrainMap.specifyPrototype(tile, tileProto);
	}

	//specifies a prototype for the torch tile
	terrainMap.specifyPrototype(TileIndex::TORCH, getTorchPrototype(terrainMap.getContent(TORCH), this));

	//sets terrainmap pointer
	m_terrainMap = &terrainMap;
}

//sets up the level
void LavaLevel::setupLevel(IceGame& game, sf::RenderTarget& window, reb::Scene& scene, reb::ContentLoader& content) {
	//sets up main material
	Material mat{ 1, 0, 1, 0.9 };

	const Rect<float> SCENEBOUNDS{ 0, 0, 2240, 1024 };
	addPlayerBounds(scene, SCENEBOUNDS);
	const Rect<float> WATERBOUNDS{ SCENEBOUNDS.left, SCENEBOUNDS.top - 100, SCENEBOUNDS.width, SCENEBOUNDS.height + 100 };

	//adds the iceblock
	m_iceblock = (Iceblock*)scene.addGameObj(new Iceblock(this, content, Vector2(32, 160)));

	//adds the sunlight
	if(true){
		GameObj* sun = scene.addGameObj(new GameObj());
		sun->setLight(new SunLight(Color(255, 255, 255), 20, Vector2(-2, 1.5)));
		sun->getTransform()->setZ(5);
	}
	
	//sets up the player and the camera
	Ghost* ghost = (Ghost*)scene.addGameObj(new Ghost(scene, content, Vector2(32, 32), window, SCENEBOUNDS));

	//adds the winning trigger
	{
		auto winningTrigger = scene.addGameObj(new LambdaTrigger(
			[this](LambdaTrigger* trigger, Container* other) {
			if (other == m_iceblock) {
				m_winView->activate();
			}
		},
			[](LambdaTrigger* trigger, Container* other) {},
			new Transform(false, false, false, Vector2(814, 995))
			));
		winningTrigger->setBody(new Rigidbody(new Convex(Rect<float>{0, 0, 143, 29}), Material{ 0, 0, 0, 0 }, 0, false));
	}

	//adds losing triggers for when the iceblock falls out of bounds
	{
		std::array<Rect<float>, 3> loseAt{
			Rect<float>{1910, 1063, 79, 11},
			Rect<float>{959, 1057, 834, 993},
			Rect<float>{238, 1062, 528, 12}
		};

		for (auto& lose : loseAt) {
			auto losingTrigger = scene.addGameObj(new LambdaTrigger(
				[this](LambdaTrigger* trigger, Container* other) {
				if (other == m_iceblock) {
					m_loseView->activate();
				}
			},
				[](LambdaTrigger* trigger, Container* other) {},
				new Transform(false, false, false, Vector2(lose.left, lose.top))
				));
			losingTrigger->setBody(new Rigidbody(new Convex(Rect<float>{0, 0, lose.width, lose.height}), Material{ 0, 0, 0, 0 }, 0, false));
		}

	}

	//adds triangle rigidbodies to lead the lava-falls above the map
	{
		std::array<std::array<Vector2, 3>, 6> triangles{
			{
				{ Vector2(271, -1), Vector2(175, -1), Vector2(175, -200) },
				{ Vector2(304, -1), Vector2(400, -200), Vector2(400, -1) },
				{ Vector2(495, -1), Vector2(399, -1), Vector2(399, -200) },
				{ Vector2(528, -1), Vector2(624, -200), Vector2(624, -1) },
				{ Vector2(687, -1), Vector2(591, -1), Vector2(591, -200) },
				{ Vector2(720, -1), Vector2(816, -200), Vector2(816, -1) }
			}
		};
		for (auto& triangle : triangles) {
			GameObj* triangleObj = scene.addGameObj(new GameObj());
			triangleObj->setBody(new Rigidbody(new Convex({ triangle[0], triangle[1], triangle[2] }), Material(1000, 0, 0, 0), 0, true));
		}
	}

	//adds a lava multi emitter for the pool and lava-falls
	if (true) {
		GameObj* water = scene.addGameObj(new HeatObj(this, new LavaHeat(2000), new Transform(false, false, false, Vector2(0, 0))));
		water->setLayer(WATER);
		auto emitter = new MultiLiquidEmitter(
			{
				MultiLiquidEmitter::SpawningArea{
					{Vector2(1376, 214), Vector2(1450, 225)}, {15, 25}, 50
				},
				MultiLiquidEmitter::SpawningArea{
					{Vector2(1376, 224), Vector2(1700, 235)}, {15, 25}, 110
				},
				MultiLiquidEmitter::SpawningArea{
					{Vector2(1700, 224), Vector2(1800, 235)}, {15, 25}, 20
				},
				MultiLiquidEmitter::SpawningArea{
					{Vector2(1800, 224), Vector2(1845, 235)}, {-1, -1}, 100
				},
				MultiLiquidEmitter::SpawningArea{
					{Vector2(282, -100), Vector2(292, -20)}, {10, 15}, 50, 30
				},
				MultiLiquidEmitter::SpawningArea{
					{Vector2(506, -100), Vector2(519, -20)}, {10, 15}, 50, 40
				},
				MultiLiquidEmitter::SpawningArea{
					{Vector2(698, -100), Vector2(711, -20)}, {10, 15}, 50, 50
				},
			},
			Color(221, 103, 38),
			430,
			20,
			WATERBOUNDS,
			6,
			Material{ 15, 0, 0, 0 },
			120, LayerIndex::WATER
		);
		water->setEmitter(
			emitter
		);
		water->setLight(new LiquidGlow(Color(255, 100, 0), 750, 100));
		water->getTransform()->setZ(-10);
		water->getLight()->setOffset(Vector3(0, 0, 15));

		LavaObject* replacementPrototype = new LavaObject(this, m_iceblock, 6, 20, Material{ 15, 0.5, 0, 0 }, 120, LayerIndex::WATER);
		emitter->setPrototype(replacementPrototype);
	}

	//adds the pipe
	{
		ParticleEmitter* pipeWater;
		//adds the top
		{
			GameObj* pipe = scene.addGameObj(
				new LambdaCollide([](Container* other) { return other->getLayer() != LayerIndex::WATER; },
					new Transform(false, false, false, Vector2(2076, 156)))
			);
			pipe->setLayer(TERRAINTOP);
			auto spr = new ShadedSprite(content.getContent(ContentIndex::PIPE_TOP));
			spr->setAlpha(5);
			pipe->setSprite(spr);
			pipe->setBody(new Rigidbody(new Concave(content.getContent(ContentIndex::PIPE_TOP)), Material(1, 0, 1, 0.9), 0, true));

		}
		//adds the bottom and the water emitter
		{
			GameObj* pipe = scene.addGameObj(new GameObj(new Transform(false, false, false, Vector2(2076, 156))));
			pipe->setSprite(new ShadedSprite(content.getContent(ContentIndex::PIPE_BOTTOM)));

			//adds water emitter to pipe
			LiquidEmitter* water = new LiquidEmitter(
				{ -1, -1 },
				Color(50, 100, 255, 180),
				300, 1,
				15,
				{ Vector2(39, 38), Vector2(55, 48) },
				WATERBOUNDS,
				4,
				Material{ 10, 1.175, 0, 0 },
				100, LayerIndex::WATER,
				5
			);
			water->setPrototype(new ObsidianLiquid(content, 4, 15, Material{ 10, 1.175, 0, 0 }, 100, LayerIndex::WATER));
			pipe->setEmitter(water);
			pipe->getEmitter()->disable();
			pipeWater = pipe->getEmitter();
		}
		//adds colliders for the water
		{
			GameObj* topCollider = scene.addGameObj(
				new LambdaCollide([](Container* other) {return other->getLayer() == LayerIndex::WATER; }, new Transform()
			));
			topCollider->setBody(new Rigidbody(
				new Concave(
					{ 
						new Convex(
							{
								Vector2(2093, 183),
								Vector2(2045, 147),
								Vector2(2171, 117),
								Vector2(2158, 170),
							}
						),
						new Convex(
							{
								Vector2(2225, 235),
								Vector2(2176, 210),
								Vector2(2158, 170),
								Vector2(2171, 117),
							}
						),
						new Convex(
							{
								Vector2(2176, 210),
								Vector2(2225, 235),
								Vector2(2093, 285),
								Vector2(2097, 228),
							}
						),
					}
				), Material(1, 0, 1, 0.9), 0, true));
		}
		//adds the button to turn the water on
		{
			GameObj* button = scene.addGameObj(new LambdaTrigger(
				[pipeWater, &content](LambdaTrigger* trigger, Container* other) {
					if (other->getLayer() == LayerIndex::ICELAYER) {
						pipeWater->enable();
						trigger->clearBody();
						((Sprite*)trigger->getSprite())->setContent(content.getContent(ContentIndex::BUTTON_PRESSED));
					}
				},
				[](LambdaTrigger* trigger, Container* other) {},
				new Transform(false, false, false, Vector2(2212, 308))
			));
			auto spr = new ShadedSprite(content.getContent(ContentIndex::BUTTON));
			button->setSprite(spr);
			button->setBody(new Rigidbody(new Convex(Rect<float>(0, 2, 5, 19)), Material(0, 0, 0, 0), 0, false));
		}
	}

	//adds the boat
	{
		//the actual boat
		GameObj* boat = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(1310, 99))));
		boat->setSprite(new ShadedSprite(content.getContent(ContentIndex::BOAT)));
		boat->setBody(new Rigidbody(new Concave(content.getContent(ContentIndex::BOAT)), Material(2, 0, 1, 0.9), 80, true));
		
		//a trigger that destroys the boat and spawns boat pieces that will be destroyed when they fall out of the scene
		GameObj* trigger = scene.addGameObj(
			new LambdaTrigger(
				//when something enters the trigger
				[boat, &scene, &content, SCENEBOUNDS](LambdaTrigger* trigger, Container* other) {
					//if the other object is the boat
					if (other == boat) {
						//gets the position of the boat
						Rect<float> bounds = other->getTransform()->getWorldSpace(Rect<float>{0, 29, 95, 11});
						//destroys the boat
						other->destroy();
						//spawns the boat pieces
						for (int k = 0; k < 25; k++) {
							ContentIndex pieceIndex = (ContentIndex)genRandi((int)BOATPIECE_0, (int)BOATPIECE_5);
							GameObj* piece = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(genRandf(bounds.left, bounds.getRight()), genRandf(bounds.top, bounds.getBottom())), 1, genRandf(0, 2.0f * PI))));
							piece->setSprite(new ShadedSprite(content.getContent(pieceIndex)));
							piece->setBody(new Rigidbody(new Concave(content.getContent(pieceIndex)), Material(1, 0, 1, 0.9), 120, true));
							piece->addComponent(new DestroyOutBounds(SCENEBOUNDS));
						}
						//destroys the trigger
						trigger->destroy();
					}
				}, 
				[](LambdaTrigger*, Container*) {}, 
				new Transform(false, false, false, Vector2(1893, 383))
			)
		);
		trigger->setBody(new Rigidbody(new Convex(Rect<float>(0, 0, 205, 126)), Material(0, 0, 0, 0), 0, false));
		
	}

	//adds the rock spawners and bridge
	{
		//adds the bridge
		GameObj* bridge;
		{
			bridge = scene.addGameObj(new GameObj(new Transform(false, false, false, Vector2(1000, 208))));
			auto spr = new ShadedSprite(content.getContent(ContentIndex::BRIDGE));
			bridge->setSprite(spr);
			bridge->setBody(new Rigidbody(new Concave(content.getContent(ContentIndex::BRIDGE)), Material(1, 0, 1, 0.9), 0, true));
		}

		//adds the main rock spawner
		ParticleEmitter* mainEmitter;
		{
			//creates the rock prototype
			RandSpriteParticleObject* rock = new RandSpriteParticleObject({
				content.getContent(ROCK_0),
				content.getContent(ROCK_1),
				content.getContent(ROCK_2),
				content.getContent(ROCK_3),
				});
			rock->setSprite(new ShadedSprite(content.getContent(ROCK_0)));
			rock->setBody(new Rigidbody(new Concave(content.getContent(ROCK_0)), Material(30, 0, 1, 0.9), 200, true));

			const float UPSPACE = 1500;

			//creates the particle prototype
			ObjParticle* particle = new ObjParticle(rock, false);
			particle->addComp(new ReversePosSync());
			particle->addComp(new DespawnBounds(Rect<float>(SCENEBOUNDS.left, SCENEBOUNDS.top - UPSPACE, SCENEBOUNDS.width, SCENEBOUNDS.height + UPSPACE)));

			//creates the particle emitter
			mainEmitter = new ParticleEmitter(
				particle,
				15, 1,
				{ -1, -1 },
				{ Vector2(0, 0), Vector2(128, UPSPACE) },
				{ 1, 1 }, { 0, 0 }
			);

			//creates the spawner object
			GameObj* spawner = scene.addGameObj(new GameObj(new Transform(false, false, false, Vector2(1008, -UPSPACE - 50))));
			spawner->setEmitter(mainEmitter);
			spawner->getEmitter()->disable();

			//gives the emitter a scene-wide AABB so it will always be updated
			mainEmitter->updateLocalAABB(Rect<float>(SCENEBOUNDS.left - 1008, SCENEBOUNDS.top + UPSPACE, SCENEBOUNDS.width + 1008, SCENEBOUNDS.height + UPSPACE));
		}
		
		//adds the rock spawners in the lava falls
		std::array<GameObj*, 3> otherEmitters;
		{
			//the positions of the rock emitters
			std::array<Vector2, 3> emitterPositions = {
				Vector2(274, -50),
				Vector2(499, -50),
				Vector2(691, -50)
			};

			//creates a prototype for the rocks
			BasicParticleObj* protoRock = new BasicParticleObj();
			protoRock->setSprite(new ShadedSprite(content.getContent(ROCK_0)));
			protoRock->setBody(new Rigidbody(new Concave(content.getContent(ROCK_0)), Material(10, 0, 1, 0.9), 150, true));

			ObjParticle* protoPart = new ObjParticle(protoRock);
			protoPart->addComp(new ReversePosSync());
			protoPart->addComp(new DespawnBounds(Rect<float>(SCENEBOUNDS.left, SCENEBOUNDS.top - 575, SCENEBOUNDS.width, SCENEBOUNDS.height + 575)));

			//adds the emitters
			for (int k = 0; k < 3; k++) {
				otherEmitters[k] = scene.addGameObj(new GameObj(new Transform(false, false, false, emitterPositions[k])));
				otherEmitters[k]->setEmitter(new ParticleEmitter(
					protoPart->clone(0, Vector2(), 0, 0),
					5, 1,
					{-1, -1},
					{ Vector2(0, -500), Vector2(26, 0) },
					{ 1, 1, },
					{ 0, 2.0f * PI },
					ParticleEmitter::PARTICLE_RENDER_TOP,
					true
				));
				otherEmitters[k]->getEmitter()->disable();
			}

			//frees the original prototype
			delete protoPart;
		}

		//creates the trigger that enables the spawner
		auto trigger = scene.addGameObj(new LambdaTrigger(
			//on trigger enter function
			[this, mainEmitter, otherEmitters, bridge, &scene, &content, SCENEBOUNDS](LambdaTrigger* trigger, Container* other) {

				//if the other object is the iceblock
				if (other == m_iceblock) {

					//enables the rock emitters
					mainEmitter->enable();
					for (auto& other : otherEmitters) { 
						if (auto emitter = other->getEmitter()) {
							emitter->enable();
						}
					}

					//destroys the bridge object
					bridge->destroy();

					//spawns bridge piece objects with random locations and rotations
					for (ContentIndex piece = BRIDGEPIECE_0; piece < BRIDGEPIECE_9; piece = (ContentIndex)((int)piece + 1)) {
						const float PADDING = 20;
						float posX = genRandf(1008 + PADDING, 1008 + 128 - PADDING);
						float posY = genRandf(210 + PADDING, 210 + 60 - PADDING);
						auto current = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(posX, posY))));
						current->setSprite(new ShadedSprite(content.getContent(piece)));
						current->setBody(new Rigidbody(new Concave(content.getContent(piece)), Material(100, 0, 1, 0.9), 120, true));
						current->getBody()->setAngularVelocity(genRandf(-15, 15));
						current->addComponent(new DestroyOutBounds(SCENEBOUNDS));
					}

					//spawns a rock around the bridge pieces that will despawn once it leaves the level
					GameObj* rock = scene.addGameObj(new GameObj(new Transform(true, false, true, Vector2(1046, 144))));
					rock->setSprite(new ShadedSprite(content.getContent(ROCK_2)));
					rock->setBody(new Rigidbody(new Concave(content.getContent(ROCK_2)), Material(30, 0, 1, 0.9), 125, true));
					rock->addComponent(new DestroyOutBounds(SCENEBOUNDS));

					//destroys the trigger
					trigger->destroy();

				}
			}, 
			[](LambdaTrigger* trigger, Container* other) {}, 
			new Transform(false, false, false, Vector2(1166, 347))
		));
		trigger->setBody(new Rigidbody(new Convex(Rect<float>(0, 0, 196, 341)), Material(0, 0, 0, 0), 0, false));
	}

	//adds triangles to lead the rocks correctly
	{
		std::array<std::array<Vector2, 3>, 2> triangles{
			{
				{
					Vector2(1007, -1),
					Vector2(807, -1),
					Vector2(807, -200)
				},
				{
					Vector2(1136, -1),
					Vector2(1336, -200),
					Vector2(1336, -1)
				},
			} 
		};
		for (auto& triangle : triangles) {
			GameObj* obj = scene.addGameObj(new GameObj());
			obj->setBody(new Rigidbody(new Convex({ triangle[0], triangle[1], triangle[2] }), Material(1, 0, 1, 0.9), 0, true));
		}
	}

	//adds a regen station
	{
		//creates the heat emitter
		TriggerHeat* coolSource = new TriggerHeat(300);
		coolSource->setCold();

		//adds the object
		GameObj* station = scene.addGameObj(new HeatObj(this, coolSource, new Transform(false, false, false, Vector2(1371, 455))));
		station->setLayer(TERRAINTOP);

		//adds the sprite
		auto spr = new ShadedSprite(content.getContent(REGENSTATION));
		spr->setAlpha(5);
		station->setSprite(spr);

		//adds the trigger
		station->setBody(new Rigidbody(new Convex(Rect<float>(0, 22, 205, 477)), Material(0, 0, 0, 0), 0, false));

		//adds the snow particles
		auto prototype = new RandSpriteParticle({ content.getContent(SNOWFLAKE_0), content.getContent(SNOWFLAKE_1), content.getContent(SNOWFLAKE_2) });
		prototype->addComp(new Particles::MoveLine({80, 100}, { 10, 20 }));
		prototype->addComp(new Particles::TimedFade({ { 10, 0.8 }, { 90, 0.8 }, { 100, 0 } }));
		prototype->addComp(new HorizontalShake({ 2, 5 }, { 2, 3 }));
		prototype->addComp(new RotationalShake({ 0.25, 0.5 }, { 1, 4 }));
		station->setEmitter(
			new ParticleEmitter(
				prototype,
				100, 1,
				{ 5, 10 },
				{ Vector2(0, 22), Vector2(205, 22) },
				{ 1, 1 },
				{ 0, 2.0f * PI },
				ParticleEmitter::ParticleRenderMode::PARTICLE_RENDER_BOTTOM
			)
		);
	}

	//adds the terrain map to the terrain layer
	m_terrainMap->addMap(&scene, "Terrain", LayerIndex::TERRAIN, Material{ 1, 0, 1, 0.8 });
	scene.getTileset(LayerIndex::TERRAIN).m_isUpdated = false;

	//adds the torches to the ice terrain layer (because the torch layer is below the terrain whoops), 
	//it doesnt matter that its an ice layer because they dont have rigidbodies anyway
	m_terrainMap->addMap(&scene, "Torches", LayerIndex::ICETERRAIN);
	scene.getTileset(LayerIndex::ICETERRAIN).m_hasCollision = false;

	//adds the drippy tiles
	m_terrainMap->addMap(&scene, "drippy", LayerIndex::TERRAINTOP, Material{ 1, 0, 1, 0.8 });

	//adds background
	Tileset& backgr = scene.addTileset(32, 32, LayerIndex::BACKGROUND);
	auto backgrSpr = new ShadedSprite(content.getContent(LAVASTONEBACKGROUND));
	auto tile = new Tile();
	tile->setSprite(backgrSpr);
	backgr.registerTile(TileIndex::LAVASTONEBACKGROUND, tile);
	backgr.setTiles({0, 69}, {0, 31}, TileIndex::LAVASTONEBACKGROUND);
	backgr.m_hasCollision = false;
	backgr.m_isUpdated = false;
}

//sets up the gui
void LavaLevel::setupGUI(IceGame& game, sf::RenderTarget& window, reb::GUI& gui, reb::ContentLoader& content) {
	setupLevelMenu(game, gui, content);
	setupIceMeter(m_iceblock, 200, gui, content);
	setupWinView(game, gui, content, LEVEL_ICE);
}

//registers the lights for the level
void LavaLevel::registerLights(reb::Shader& shader) {
	shader.addLightType<SunLight>(2);
	shader.addLightType<LiquidGlow>(1);
	shader.addLightType<PointLight>(13);
}