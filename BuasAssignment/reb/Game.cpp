#include "Game.h"
#include <random>

#include "SunLight.h"
#include "PointLight.h"

namespace reb {

	//initalizes the global config object
	Config config;

	//called by main to start the game, sets up the renderWindow and starts the game loop
	void Game::startGame(){

		//verifies the game hasnt been started yet
		if (!m_hasStarted) {

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
				config.getConfigValue<bool>(Config::configKey::FULLSCREEN) ? sf::Style::Fullscreen : sf::Style::Default
			);

			//sets the camera information
			m_window.setView(sf::View(
				sf::FloatRect(
					0, 0,
					config.getConfigValue<float>(Config::configKey::CAMERA_WIDTH),
					config.getConfigValue<float>(Config::configKey::CAMERA_HEIGHT)
				)
			));

			//letterboxes
			letterbox();

			//creates initial layers
			m_renderer.createInitialLayers(m_window);

			//prevents the game from being started again
			m_hasStarted = true;
			//initializes the game
			initialize();
			//launches the threadless game loop
			launchThreadless();

		}

	}

	//initializes the game, currently does nothing, but will eventually be used to load initial resources like a splash screen
	void Game::initialize() {
		writeLn("Initializing game");
		//adds game context with close event
		m_gameContext->addEvent(sf::Event::EventType::Closed, new Event([](sf::Event&) { return true; }, [this](sf::Event&, float) { endGame(); }));
		//adds a resize event for letterboxing
		m_gameContext->addEvent(sf::Event::EventType::Resized, new Event([](sf::Event&) { return true; }, [this](sf::Event&, float) { letterbox(); }));
		//sets the initial input contexts
		m_input.m_gameContext = m_gameContext;
		m_input.m_sceneContext = m_scene->m_sceneContext;
		m_input.m_GuiContext = m_GUI->m_guiContext;
		//requests window focus
		m_window.requestFocus();
		//calls postInitialize
		postInitialize();
	}

	//launches a threadless game loop, uses the previously found desired intervals for frames and updates
	//to call the render and update functions at the appropriate times, ends when the continueGame boolean is set to false
	void Game::launchThreadless() {
		writeLn("Launching threadless loop");

		//builds shader
		m_renderer.m_shader->buildShader();

		//syncs input contexts
		syncInputContexts();

		//sets up interval information
		auto lastRender = std::chrono::steady_clock::now();
		auto lastUpdate = std::chrono::steady_clock::now();
		float renderLag = 0;
		float updateLag = 0;

		//starts loop
		do {
			//gets the elapsed time since the last update and render
			auto now = std::chrono::steady_clock::now();
			float elapsedRender = std::chrono::duration<float>(now - lastRender).count();
			float elapsedUpdate = std::chrono::duration<float>(now - lastUpdate).count();

			//sets the last times to the current times
			lastRender = now;
			lastUpdate = now;

			//increments lag values
			renderLag += elapsedRender;
			updateLag += elapsedUpdate;

			//updates as many times as necessary to catch up with the lag
			while (updateLag >= UPDATEINTV) {
				update();
				updateLag -= UPDATEINTV;
			}
			//renders if necessary
			if (renderLag >= FRAMEINTV) {
				render();
				renderLag = 0;
			}

		}//maintains loop until continueGame is set to false
		while (m_continueGame);

		//calls onGameClose
		onGameClose();
	}

	//updates the game
	void Game::update() {

		//updates the scene if necessary
		updateScene();

		//calls pre-update
		preUpdate(UPDATEINTV);

		//updates the input
		m_input.update(m_window, UPDATEINTV);

		if (!m_GUI->m_scenePaused) {
			//updates the scene
			m_scene->update(UPDATEINTV);
		}

		//updates the gui
		if (m_window.hasFocus()) {
			m_GUI->update(UPDATEINTV);
		}

		//calls post-update
		postUpdate(UPDATEINTV);

		//gets the actual current ups
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration<float>(now - m_lastUpdate).count();
		m_lastUpdate = now;
		m_ups = 1 / elapsed;
	}

	//renders the current game state to the window
	void Game::render() {
		//clears the window
		m_window.clear();

		//calls preRender
		preRender();

		//renders
		m_renderer.render(m_window, *m_GUI);

		//calls postRender
		postRender();

		//displays the game
		m_window.display();

		//gets the actual current fps
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration<float>(now - m_lastRender).count();
		m_lastRender = now;
		m_fps = 1 / elapsed;
	}

	//ends the game by setting the continueGame boolean to false
	void Game::endGame() {
		writeLn("Ending game");
		m_continueGame = false;
	}

	//replaces the current scene with the given scene
	void Game::setScene(SceneFunction function) {
		//creates new contentloader and gui
		ContentLoader* newLoader = new ContentLoader();
		GUI* newGUI = new GUI();
		Shader* newShader = new Shader();

		//executes the function
		Scene* newScene = function(*newLoader, *newGUI, *newShader);

		//builds the shader
		newShader->buildShader();

		//sets the new scene
		if (m_newScene != nullptr) {
			delete m_newScene;
		}
		m_newScene = newScene;
		
		//sets the new contentloader
		if (m_newLoader != nullptr) {
			delete m_newLoader;
		}
		m_newLoader = newLoader;

		//sets the new gui
		if (m_newGUI != nullptr) {
			delete m_newGUI;
		}
		m_newGUI = newGUI;

		//sets the new shader
		if (m_newShader != nullptr) {
			delete m_newShader;
		}
		m_newShader = newShader;

	}

	//updates the scene if necessary
	void Game::updateScene() {
		//updates the scene if necessary
		if (m_newScene != nullptr) {
			delete m_scene;
			m_scene = m_newScene;
			m_renderer.m_scene = m_newScene;
			m_newScene = nullptr;
			m_scene->finalAddition();
			//updates the window view
			m_window.setView(sf::View(sf::FloatRect(0, 0, m_window.getView().getSize().x, m_window.getView().getSize().y)));
		}

		//updates the contentloader if necessary
		if (m_newLoader != nullptr) {
			delete m_content;
			m_content = m_newLoader;
			m_newLoader = nullptr;
		}

		//updates the GUI if necessary
		if (m_newGUI != nullptr) {
			delete m_GUI;
			m_GUI = m_newGUI;
			m_newGUI = nullptr;
			m_GUI->update(0);
			m_GUI->update(0);
			m_GUI->update(0);
		}

		//updates the shader if necessary
		if (m_newShader != nullptr) {
			delete m_renderer.m_shader;
			m_renderer.m_shader = m_newShader;
			m_newShader = nullptr;
		}

		//syncs input contexts for the new scene and gui
		syncInputContexts();

		//letterboxes (not sure why this is necessary here but it's not like it takes a long time anyway)
		letterbox();
	}

	//syncronizes the input contexts of the scene and gui with the input
	void Game::syncInputContexts() {
		//syncs scene input
		if (m_input.m_sceneContext != m_scene->m_sceneContext) {
			m_input.m_sceneContext = m_scene->m_sceneContext;
		}
		//syncs gui input
		if (m_input.m_GuiContext != m_GUI->m_guiContext) {
			m_input.m_GuiContext = m_GUI->m_guiContext;
		}
	}

	//sets the content loader to the given one
	void Game::setContentLoader(ContentLoader& other) {
		if (m_newLoader != nullptr) {
			delete m_newLoader;
		}
		m_newLoader = &other;
	}

	//gives access to the game context to child classes
	Context& Game::getGameContext() {
		return *m_gameContext;
	}

	//letterboxes the current view
	void Game::letterbox() {
		//gets the size of the view
		sf::View view = m_window.getView();
		Vector2 viewSize = view.getSize();

		//gets the size of the window
		Vector2 windowSize = Vector2(m_window.getSize().x, m_window.getSize().y);

		//gets the size that would be used if the width would be filled
		float xDivY = viewSize.x / viewSize.y;
		Vector2 fullWidth(windowSize.x, windowSize.x / (xDivY));

		//checks if this size fits
		if (fullWidth.y <= windowSize.y) {
			//scales the viewport
			Vector2 viewportDim = fullWidth.compDiv(Vector2(viewSize.x, viewSize.y));
			Rect<float> newSize{ 0, 0, viewportDim.x * viewSize.x, viewportDim.y * viewSize.y };
			//centers viewport vertically
			newSize.top = (windowSize.y - newSize.height) * 0.5f;
			view.setViewport(sf::FloatRect(
				newSize.left / windowSize.x, 
				newSize.top / windowSize.y, 
				newSize.width / windowSize.x, 
				newSize.height / windowSize.y)
			);
		}
		//uses a size with filled height if it doesnt fit
		else {
			//gets the size that would be used if the height would be filled
			Vector2 fullHeight(windowSize.y * xDivY, windowSize.y);
			//scales the viewport
			Vector2 viewportDim = fullHeight.compDiv(Vector2(viewSize.x, viewSize.y));
			Rect<float> newSize{ 0, 0, viewportDim.x * viewSize.x, viewportDim.y * viewSize.y };
			//centers viewport horizontally
			newSize.left = (windowSize.x - newSize.width) * 0.5f;
			view.setViewport(sf::FloatRect(
				newSize.left / windowSize.x,
				newSize.top / windowSize.y,
				newSize.width / windowSize.x,
				newSize.height / windowSize.y)
			);
		}

		//sets the view
		m_window.setView(view);
	}

	//destructor
	Game::~Game() {
		//calls update one more time if the new scene or the new loader was set to safely remove these as well
		if (m_newScene != nullptr || m_newLoader != nullptr) {
			update();
		}
		//frees memory of the scene and content loader
		delete m_scene;
		delete m_content;
		//frees memory of the gui
		delete m_GUI;
		//frees memory of the game input context
		delete m_gameContext;
	}

}