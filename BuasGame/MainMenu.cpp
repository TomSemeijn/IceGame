#include "MainMenu.h"

#include <reb/ShadedSprite.h>
#include <reb/Concave.h>

#include <reb/PointLight.h>
#include <reb/SunLight.h>

#include <reb/Views/SpriteComp.h>
#include <reb/Views/SpriteButton.h>

#include "ContentIndex.h"
#include "TileIndex.h"
#include "LayerIndex.h"

#include "GeneralFactory.h"

#include <reb/PixelParticle.h>
#include <reb/Particles/Fade.h>
#include <reb/Particles/ColorFade.h>
#include <reb/Particles/MoveLine.h>
#include <reb/ParticleEmitter.h>

using namespace reb;

extern sf::Font mainFont;

//loads the content for the main menu
void MainMenu::loadContent(reb::ContentLoader& content) {
	content.loadContent(ContentIndex::GAME_TITLE, "Textures/GUI/Title");
	content.loadContent(ContentIndex::LEVELVIEW, "Textures/GUI/levelView");
	content.loadContent(ContentIndex::MENUBUTTON_CLICK, "Textures/GUI/menubutton_clicked");
	content.loadContent(ContentIndex::MENUBUTTON_HOVER, "Textures/GUI/menubutton_hover");
	content.loadContent(ContentIndex::MENUBUTTON_IDLE, "Textures/GUI/menubutton_idle");
	content.loadContent(ContentIndex::LEVEL_THUMBNAIL_1, "Textures/GUI/levelThumbnail_1");
	content.loadContent(ContentIndex::LEVEL_THUMBNAIL_2, "Textures/GUI/levelThumbnail_2");
	content.loadContent(ContentIndex::LEVEL_THUMBNAIL_3, "Textures/GUI/levelThumbnail_3");
	content.loadContent(TileIndex::CAVEBACKGROUND_0, "Textures/Cave_backgr_0");
	content.loadContent(TileIndex::TORCH, "Textures/torch");
	content.loadContent(ContentIndex::BLOB, "Textures/blob");
}

//sets up the level part of the menu
void MainMenu::setupLevel(IceGame& game, sf::RenderTarget& window, reb::Scene& scene, reb::ContentLoader& content) {

	//sets the background
	Tileset& backgroundSet = scene.addTileset(32, 32, BACKGROUND);
	auto tile = new Tile();
	tile->setSprite(new ShadedSprite(content.getContent(CAVEBACKGROUND_0)));
	backgroundSet.registerTile(CAVEBACKGROUND_0, tile);
	backgroundSet.setTiles({ 0, 15 }, { 0, 9 }, CAVEBACKGROUND_0);

	//adds an object particle emitter
	addFireflyEmitter(scene, content.getContent(BLOB), 15, { 5, 15 }, { Vector2(50, 50), Vector2(430, 220) });

	//sets torches
	Tileset& torchSet = scene.addTileset(32, 32, MIDDLEGROUND);
	torchSet.registerTile(TORCH, getTorchPrototype(content.getContent(TORCH)));
	torchSet.setTiles({
		{2, 1},
		{12, 1},
		{7, 3}
	}, TORCH);

	//adds the title
	GameObj* gameTitle = scene.addGameObj(new GameObj(new Transform(true, false, false, Vector2(), 0.5), Container::BOUNDS_SPRITE));
	gameTitle->setSprite(new ShadedSprite(content.getContent(GAME_TITLE)));
	gameTitle->setLayer(LayerIndex::FOREGROUND);
	((ShadedSprite*)gameTitle->getSprite())->setAlpha(5);
	gameTitle->getTransform()->setZ(-5);
	Vector2 worldCenter(240, 50);
	Vector2 titleWorldCenter = gameTitle->getTransform()->getWorldSpace(gameTitle->getTransform()->getBounds().getCenter());
	gameTitle->getTransform()->setPos(worldCenter - titleWorldCenter);

	//adds a sunlight
	GameObj* sun = scene.addGameObj(new GameObj());
	sun->getTransform()->setZ(5);
	sun->setLight(new SunLight(Color(255, 255, 255), 30, Vector2(-2, 1)));
}

//sets up the gui for the menu
void MainMenu::setupGUI(IceGame& game, sf::RenderTarget& window, reb::GUI& gui, reb::ContentLoader& content) {

	//adds main view
	View* mainView = gui.addView(new View(Vector2(gui.getSize().x, gui.getSize().y), nullptr, Vector2(), false, true));

	//adds level views
	addLevelView(mainView, game, gui, content, ContentIndex::LEVEL_THUMBNAIL_1, LevelIndex::LEVEL_CAVE, Vector2(128, 290));
	addLevelView(mainView, game, gui, content, ContentIndex::LEVEL_THUMBNAIL_2, LevelIndex::LEVEL_LAVA, Vector2(565, 290));
	addLevelView(mainView, game, gui, content, ContentIndex::LEVEL_THUMBNAIL_3, LevelIndex::LEVEL_ICE, Vector2(1002, 290));

	//adds quit game button
	Views::SpriteComp* sprComp = new Views::SpriteComp(content.getContent(MENUBUTTON_IDLE));
	sprComp->setColor(Color(255, 0, 0, 200));
	Views::SpriteButton* button = new Views::SpriteButton(sprComp, content.getContent(MENUBUTTON_HOVER), content.getContent(MENUBUTTON_CLICK), [&game]() { game.endGame(); }, Vector2(1036, 693));
	button->setScale(3);
	Views::TextComp* text = new Views::TextComp(mainFont, 11, false, Color(0, 0, 0), "Quit game");
	text->setPaddingLeft(5);
	text->setPaddingTop(5);
	button->addComp(text);
	mainView->addChild(button);

}

//adds a levelview with the given thumbnail and levelIndex
void MainMenu::addLevelView(reb::View* mainView, IceGame& game, reb::GUI& gui, reb::ContentLoader& content, reb::ContentIndex thumbnailIndex, LevelIndex level, reb::Vector2 viewPos) {
	//adds the root level view
	View* levelView = mainView->addChild(new View(Vector2(310, 255), nullptr, viewPos));
	levelView->addComp(new Views::SpriteComp(content.getContent(ContentIndex::LEVELVIEW), Color(0, 255, 255, 200)));

	//creates the play button
	Views::SpriteButton* playButton = new Views::SpriteButton(
		new Views::SpriteComp(content.getContent(ContentIndex::MENUBUTTON_IDLE), Color(0, 255, 255, 200)),
		content.getContent(ContentIndex::MENUBUTTON_HOVER),
		content.getContent(ContentIndex::MENUBUTTON_CLICK),
		[&game, level]() {
			game.setLevel(level);
		},
		Vector2(45, 183)
	);
	playButton->setScale(2);
	//adds text to the play button
	Views::TextComp* buttonText = new Views::TextComp(mainFont, 14, true, Color(0, 0, 0, 200), "Play");
	buttonText->setPaddingLeft(40);
	buttonText->setPaddingTop(4);
	playButton->addComp(buttonText);
	//adds the play button
	levelView->addChild(playButton);

	//adds the thumbnail
	Views::SpriteComp* thumbnail = new Views::SpriteComp(content.getContent(thumbnailIndex), Color(255, 255, 255), Views::SpriteComp::SpriteScaleType::SPRITE_NOSCALE);
	thumbnail->setPaddingLeft(49);
	thumbnail->setPaddingTop(35);
	levelView->addComp(thumbnail);
}

//registers the lights used in the menu
void MainMenu::registerLights(reb::Shader& shader) {
	shader.addLightType<SunLight>(1);
	shader.addLightType<PointLight>(18);
}