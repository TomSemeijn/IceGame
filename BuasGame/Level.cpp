#include "Level.h"
#include <reb/Views/SpriteButton.h>
#include <reb/Concave.h>
#include "ContentIndex.h"
#include "HeatHolder.h"
#include "HeatEmitter.h"
#include "Icemeter.h"
#include "LayerIndex.h"
#include "LambdaCollide.h"

using namespace reb;

extern sf::Font mainFont;

//starts the level for the first time
reb::Scene* Level::startLevel(IceGame& game, sf::RenderTarget& window, reb::ContentLoader& content, reb::GUI& gui, reb::Shader& shader, reb::Views::WriteTextComp* progressReport) {
	//sets members
	m_scene = new reb::Scene();
	m_GUI = &gui;
	m_content = &content;
	m_game = &game;
	m_window = &window;

	//loads content
	if (progressReport != nullptr) {
		progressReport->setText("Loading content...");
	}
	loadContent(content);

	//registers lights
	if (progressReport != nullptr) {
		progressReport->setText("Registering lights...");
	}
	registerLights(shader);

	//sets up level
	if (progressReport != nullptr) {
		progressReport->setText("Setting up level...");
	}
	setupLevel(game, window, *m_scene, content);

	//sets up gui
	if (progressReport != nullptr) {
		progressReport->setText("Setting up GUI...");
	}
	setupGUI(game, window, gui, content);

	//manages content
	if (progressReport != nullptr) {
		progressReport->setText("Managing content...");
	}
	m_content->manage();

	//returns scene
	return m_scene;
}

//actually restarts the level at the end of a game loop
void Level::actuallyRestart() {
	//clears the level
	m_scene->clearScene();
	//clears the gui
	m_GUI->clearGUI();
	//clears the heat objects
	m_heatObjects.clear();

	//sets up the level
	setupLevel(*m_game, *m_window, *m_scene, *m_content);
	//sets up the gui
	setupGUI(*m_game, *m_window, *m_GUI, *m_content);
}

//restarts the level without needing to reload content
void Level::restartLevel() {
	//tells the game to restart the level at the end a game loop
	m_game->restartCurrentLevel();
}

//sets up the menu used in all normal levels
void Level::setupLevelMenu(IceGame& game, reb::GUI& gui, reb::ContentLoader& content) {
	content.loadContent(ContentIndex::MENUBUTTON_CLICK, "Textures/GUI/menubutton_clicked");
	content.loadContent(ContentIndex::MENUBUTTON_HOVER, "Textures/GUI/menubutton_hover");
	content.loadContent(ContentIndex::MENUBUTTON_IDLE, "Textures/GUI/menubutton_idle");
	content.loadContent(ContentIndex::TITLEPANEL, "Textures/GUI/titlePanel");

	View* menuView = new View(Vector2(gui.getSize().x, gui.getSize().y), nullptr, Vector2(), true, true);
	gui.addView(menuView);
	menuView->deactivate();

	gui.getGuiContext().addEvent(sf::Event::EventType::KeyPressed, new Event(
		[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::Key::Escape;
		},
		[menuView](sf::Event&, float) {
			menuView->toggleActive();
		}
	));

	View* titleView = new View(Vector2(235, 35), nullptr, Vector2(250, 30));
	titleView->setScale(4);
	Views::TextComp* text = new Views::TextComp(mainFont, 12, false, Color(0, 0, 0), "Icegame");
	text->setPaddingLeft(15);
	text->setPaddingTop(8);
	titleView->addComp(new Views::SpriteComp(content.getContent(TITLEPANEL)));
	titleView->addComp(text);
	menuView->addChild(titleView);

	{
		Views::SpriteButton* button = new Views::SpriteButton(new Views::SpriteComp(content.getContent(MENUBUTTON_IDLE)), content.getContent(MENUBUTTON_HOVER), content.getContent(MENUBUTTON_CLICK), [menuView]() { menuView->deactivate(); }, Vector2(500, 220));
		button->setScale(4);
		Views::TextComp* text = new Views::TextComp(mainFont, 8, false, Color(0, 0, 0), "Continue");
		text->setPaddingLeft(5);
		text->setPaddingTop(5);
		button->addComp(text);
		menuView->addChild(button);
	}

	{
		Views::SpriteButton* button = new Views::SpriteButton(new Views::SpriteComp(content.getContent(MENUBUTTON_IDLE)), content.getContent(MENUBUTTON_HOVER), content.getContent(MENUBUTTON_CLICK), [this]() { restartLevel(); }, Vector2(500, 370));
		button->setScale(4);
		Views::TextComp* text = new Views::TextComp(mainFont, 8, false, Color(0, 0, 0), "Restart level");
		text->setPaddingLeft(5);
		text->setPaddingTop(5);
		button->addComp(text);
		menuView->addChild(button);
	}

	{
		Views::SpriteButton* button = new Views::SpriteButton(
			new Views::SpriteComp(content.getContent(MENUBUTTON_IDLE)), 
			content.getContent(MENUBUTTON_HOVER), 
			content.getContent(MENUBUTTON_CLICK), 
			[&game]() { game.setLevel(LevelIndex::NOLEVEL_MAIN); },
			Vector2(500, 520)
		);
		button->setScale(4);
		Views::TextComp* text = new Views::TextComp(mainFont, 8, false, Color(0, 0, 0), "Main menu");
		text->setPaddingLeft(5);
		text->setPaddingTop(5);
		button->addComp(text);
		menuView->addChild(button);
	}

	{
		Views::SpriteComp* sprComp = new Views::SpriteComp(content.getContent(MENUBUTTON_IDLE));
		sprComp->setColor(Color(255, 0, 0));
		Views::SpriteButton* button = new Views::SpriteButton(sprComp, content.getContent(MENUBUTTON_HOVER), content.getContent(MENUBUTTON_CLICK), [&game]() { game.endGame(); }, Vector2(1100, 700));
		button->setScale(3);
		Views::TextComp* text = new Views::TextComp(mainFont, 11, false, Color(0, 0, 0), "Quit game");
		text->setPaddingLeft(5);
		text->setPaddingTop(5);
		button->addComp(text);
		menuView->addChild(button);
	}
}

//sets up the ice meter used in all normal levels
void Level::setupIceMeter(Iceblock* iceblock, float maxColorHeat, reb::GUI& gui, reb::ContentLoader& content) {
	View* meterView = gui.addView(new View(Vector2(50, 200), nullptr, Vector2(30, 30)));
	meterView->addComp(new Icemeter(iceblock, maxColorHeat));
}

//sets up the win view in all normal levels
void Level::setupWinView(IceGame& game, reb::GUI& gui, reb::ContentLoader& content, LevelIndex nextLevel) {

	//sets up the win view
	{
		//creates and deactivates the view
		m_winView = gui.addView(new View(Vector2(814, 320), nullptr, Vector2(313, 182), true, true));
		m_winView->deactivate();

		//gives it a rectangle and text
		m_winView->addComp(new Views::RectComp(Color(86, 215, 255, 200), Color(63, 157, 186), 4));
		auto text = new Views::TextComp(mainFont, 84, false, Color(0, 0, 0, 200), "You won!");
		text->setPaddingLeft(230);
		text->setPaddingTop(36);
		m_winView->addComp(text);

		//adds replay button
		{
			Views::SpriteButton* replayButton = new Views::SpriteButton(
				new Views::SpriteComp(
					content.getContent(MENUBUTTON_IDLE)
				),
				content.getContent(MENUBUTTON_HOVER),
				content.getContent(MENUBUTTON_CLICK),
				[this]() { restartLevel(); },
				Vector2(25, 220)
			);
			replayButton->setScale(3);
			Views::TextComp* replayText = new Views::TextComp(mainFont, 8, false, Color(0, 0, 0), "Replay level");
			replayText->setPaddingLeft(5);
			replayText->setPaddingTop(5);
			replayButton->addComp(replayText);
			m_winView->addChild(replayButton);
		}

		//adds next level button (main menu if last level)
		{
			Views::SpriteButton* replayButton = new Views::SpriteButton(
				new Views::SpriteComp(
					content.getContent(MENUBUTTON_IDLE)
				),
				content.getContent(MENUBUTTON_HOVER),
				content.getContent(MENUBUTTON_CLICK),
				[&game, nextLevel]() { game.setLevel(nextLevel); },
				Vector2(460, 220)
			);
			replayButton->setScale(3);
			Views::TextComp* replayText = new Views::TextComp(mainFont, 8, false, Color(0, 0, 0), nextLevel == NOLEVEL_MAIN ? "Main menu" : "Next level");
			replayText->setPaddingLeft(5);
			replayText->setPaddingTop(5);
			replayButton->addComp(replayText);
			m_winView->addChild(replayButton);
		}
	}
	
	//sets up the lose view
	{
		//creates and deactivates the view
		m_loseView = gui.addView(new View(Vector2(814, 320), nullptr, Vector2(313, 182), true, true));
		m_loseView->deactivate();

		//gives it a rectangle and text
		m_loseView->addComp(new Views::RectComp(Color(255, 137, 86, 200), Color(211, 113, 71), 4));
		auto text = new Views::TextComp(mainFont, 84, false, Color(0, 0, 0, 200), "You lost!");
		text->setPaddingLeft(230);
		text->setPaddingTop(36);
		m_loseView->addComp(text);

		//adds replay button
		{
			Views::SpriteButton* replayButton = new Views::SpriteButton(
				new Views::SpriteComp(
					content.getContent(MENUBUTTON_IDLE)
				),
				content.getContent(MENUBUTTON_HOVER),
				content.getContent(MENUBUTTON_CLICK),
				[this]() { restartLevel(); },
				Vector2(25, 220)
			);
			replayButton->setScale(3);
			Views::TextComp* replayText = new Views::TextComp(mainFont, 8, false, Color(0, 0, 0), "Replay level");
			replayText->setPaddingLeft(5);
			replayText->setPaddingTop(5);
			replayButton->addComp(replayText);
			m_loseView->addChild(replayButton);
		}

		//adds next level button (main menu if last level)
		{
			Views::SpriteButton* replayButton = new Views::SpriteButton(
				new Views::SpriteComp(
					content.getContent(MENUBUTTON_IDLE)
				),
				content.getContent(MENUBUTTON_HOVER),
				content.getContent(MENUBUTTON_CLICK),
				[&game, nextLevel]() { game.setLevel(nextLevel); },
				Vector2(460, 220)
			);
			replayButton->setScale(3);
			Views::TextComp* replayText = new Views::TextComp(mainFont, 8, false, Color(0, 0, 0), nextLevel == NOLEVEL_MAIN ? "Main menu" : "Skip level");
			replayText->setPaddingLeft(5);
			replayText->setPaddingTop(5);
			replayButton->addComp(replayText);
			m_loseView->addChild(replayButton);
		}

	}

}

//activates the losing view
void Level::lose() {
	if (m_loseView != nullptr) {
		m_loseView->activate();
	}
}

//returns the heat on the given iceblock
float Level::getHeat(Iceblock* block) {
	float total = AMBIENTHEAT;
	for (auto& source : m_heatObjects) {
		if (auto heat = source->getHeat()) {
			total += heat->getHeatAt(block);
		}
	}
	return total;
}

//adds bounding boxes for the player around the given scene bounds
void Level::addPlayerBounds(reb::Scene& scene, const reb::Rect<float> sceneBounds) {

	//adds the object
	GameObj* bounds = scene.addGameObj(
		new LambdaCollide(
			[](Container* other) {return other->getLayer() == LayerIndex::GHOSTLAYER; }, 
			new Transform()
		)
	);

	//sets the layer to icelayer so the ghost will also collide with it
	bounds->setLayer(LayerIndex::ICELAYER);

	//adds the body
	const float MARGIN = 32;
	const float OUTWARDS = 1000;
	Rect<float> newBounds{ sceneBounds.left - MARGIN, sceneBounds.top - MARGIN, sceneBounds.width + MARGIN * 2.0f, sceneBounds.height + MARGIN * 2.0f };
	bounds->setBody(new Rigidbody(
		new Concave(
			{
				new Convex(Rect<float>{newBounds.left - OUTWARDS, newBounds.top, OUTWARDS, newBounds.height}),
				new Convex(Rect<float>{newBounds.getRight(), newBounds.top, OUTWARDS, newBounds.height}),
				new Convex(Rect<float>{newBounds.left, newBounds.getBottom(), newBounds.width, OUTWARDS}),
				new Convex(Rect<float>{newBounds.left, newBounds.top - OUTWARDS, newBounds.width, OUTWARDS})
			}
		), Material(10000, 0, 0, 0), 0, true
	));

}