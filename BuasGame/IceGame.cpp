#include "IceGame.h"
#include "LayerIndex.h"
#include "CaveLevel.h"
#include "LavalLevel.h"
#include "IceLevel.h"
#include "MainMenu.h"

#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_print.hpp>

using namespace reb;

//gets called after the game has been initialized
void IceGame::postInitialize() {

	//adds an input event to toggle fullscreen
	getGameContext().addEvent(sf::Event::KeyPressed, new Event(
		[](sf::Event& ev) {
			return ev.key.code == sf::Keyboard::F11;
		},
		[this](sf::Event&, float) {
			m_updateFullScreen = true;
		}
	));

	//sets the window icon
	m_icon.loadFromFile("Content/icon.png");
	m_window.setIcon(m_icon.getSize().x, m_icon.getSize().y, m_icon.getPixelsPtr());

	//registers levels
	m_levels.emplace(std::make_pair(LevelIndex::NOLEVEL_MAIN, new MainMenu()));
	m_levels.emplace(std::make_pair(LevelIndex::LEVEL_CAVE, new CaveLevel()));
	m_levels.emplace(std::make_pair(LevelIndex::LEVEL_LAVA, new LavaLevel()));
	m_levels.emplace(std::make_pair(LevelIndex::LEVEL_ICE, new IceLevel()));

	m_window.setVerticalSyncEnabled(true);

	Shader::s_ambientIntensity = 0.4;
	Shader::s_ambientColor = Color(255, 255, 255);

	//sets the default layer
	Container::s_defaultLayer = LayerIndex::DEFAULT;

	//sets the first level
	setLevel(NOLEVEL_MAIN);
	
}

//gets called at the start of every update
void IceGame::preUpdate(float elapsed) {

}

//gets called at the end of every update
void IceGame::postUpdate(float elapsed) {
	if (m_fps < (1 / UPDATEINTV) - 5) {
		//writeLn("LOW FPS ", m_fps);
	}
	//joins setup thread if it was running and is now finished
	if (m_setupThread.joinable() && !m_setupRunning) {
		m_setupThread.join();
	}
	//updates fullscreen mode if requested
	if (m_updateFullScreen) {

		m_updateFullScreen = false;
		//toggles the fullscreen boolean
		m_currentFullScreen = !m_currentFullScreen;
		//keeps the view of the window
		sf::View oldView = m_window.getView();

		//creates the window
		m_window.create(
			//sets the width and height of the window from the config using sfml's videomode
			sf::VideoMode(
				config.getConfigValue<int>(Config::configKey::WINDOW_WIDTH),
				config.getConfigValue<int>(Config::configKey::WINDOW_HEIGHT)
			),
			//adds the window title from the config
			config.getConfigValue<std::string>(Config::configKey::WINDOW_TITLE),
			//sets the window to fullscreen if the config demands it using sfml's window styles
			m_currentFullScreen ? sf::Style::Fullscreen : sf::Style::Default
		);

		//sets the camera information
		m_window.setView(oldView);

		//sets the icon
		m_window.setIcon(m_icon.getSize().x, m_icon.getSize().y, m_icon.getPixelsPtr());

		//letterboxes
		letterbox();

	}

}

//gets called before stuff is rendered to the window
void IceGame::preRender() {
	
}

//gets called after stuff is rendered to the window
void IceGame::postRender() {
	//restarts the current level if requested
	if (m_restart) {
		m_restart = false;
		m_levels.at(m_currentLevel)->actuallyRestart();
	}

}

//gets called when the game closes
void IceGame::onGameClose() {
	saveFullScrConfig();
}

//switches to a new level
void IceGame::setLevel(LevelIndex level) {
	//verifies the setup thread isnt already running and that the current level isnt the requested one
	if (!m_setupRunning && m_currentLevel != level) {
		//sets up the loading screen
		setScene([this](ContentLoader& loader, GUI& newGui, Shader& newShader) { return setupLoadingScreen(loader, newGui, newShader); });
		m_currentLevel = NOLEVEL_LOADING;

		//sets up the level
		m_setupRunning = true;
		m_setupThread = std::thread([this, level]() {
			setScene([this, level](ContentLoader& loader, GUI& newGui, Shader& newShader) {
				return m_levels.at(level)->startLevel(*this, m_window, loader, newGui, newShader, m_setupTextComp);
			});
			m_currentLevel = level;
			m_setupRunning = false;
		});
	}
}

//makes the current level restart
void IceGame::restartCurrentLevel() {
	m_restart = true;
}

//saves the current fullscreen setting to the config file
void IceGame::saveFullScrConfig() {
	//gets the xml file
	rapidxml::file<> file("Content/Configs/config.xml");
	//parses the xml file
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());
	//finds the root node
	rapidxml::xml_node<>* root = doc.first_node("config");
	//finds the fullscreen node
	rapidxml::xml_node<>* fullscr = root->first_node("val");
	while (std::string(fullscr->first_attribute("name")->value()) != "fullScr") { fullscr = fullscr->next_sibling("val"); }
	if (fullscr == nullptr) { writeLn("fullscreen node not found, couldn't save configuration"); return; }
	//changes the value
	fullscr->first_attribute("value")->value(m_currentFullScreen ? "true" : "false");
	//saves the modified xml file

	std::string xmlResult;
	rapidxml::print(std::back_inserter(xmlResult), doc, rapidxml::print_no_indenting);
	doc.clear();

	//puts the result into a file
	std::ofstream file_stored("Content/Configs/config.xml");
	file_stored << xmlResult;
	file_stored.close();
}

//destructor
IceGame::~IceGame() {
	//joins the setupthread
	if (m_setupThread.joinable()) {
		m_setupThread.join();
	}
	//frees memory of every registered level
	for (auto& lvl : m_levels) {
		delete lvl.second;
	}
}