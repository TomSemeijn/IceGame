#ifndef REBOOTGAME_H
#define REBOOTGAME_H

#pragma once
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>
#include "Config.h"
#include "Functions.h"
#include "Scene.h"
#include "Renderer.h"
#include "Input.h"
#include "ContentLoader.h"
#include "GUI.h"

namespace reb {

	//global config object
	extern Config config;

	//the game class initiates, maintains and closes the game loop. 
	class Game {
	private:
		//the scene and renderer
		Scene* m_scene = new Scene();
		Renderer m_renderer;

		//the scene and such to switch to
		Scene* m_newScene = nullptr;
		ContentLoader* m_newLoader = nullptr;
		GUI* m_newGUI = nullptr;
		Shader* m_newShader = nullptr;

		//the content manager
		ContentLoader* m_content = new ContentLoader();

		//the GUI
		GUI* m_GUI = new GUI();

		//the input context of the game
		Context* m_gameContext = new Context();

		//typedef for the function used to set a scene
		typedef std::function<Scene*(ContentLoader&, GUI&, Shader&)> SceneFunction;

		//updates the scene if necessary
		void updateScene();

		//syncronizes the input contexts of the scene and gui with the input
		void syncInputContexts();

	protected:
		//the input manager
		Input m_input;

		//the window the game takes place in
		sf::RenderWindow m_window;

		//a boolean for if the game should continue after the current instance of the game loop
		bool m_continueGame{ true };

		//the current fps and ups, initialized at zero
		float m_fps{ 0 };
		float m_ups{ 0 };

		//the desired frame and update intervals, set from config
		const float UPDATEINTV;
		const float FRAMEINTV;

		//the last update and render moments, initialized to the moment of construction
		std::chrono::time_point<std::chrono::steady_clock> m_lastUpdate = std::chrono::steady_clock::now();
		std::chrono::time_point<std::chrono::steady_clock> m_lastRender = std::chrono::steady_clock::now();

		//used to prevent the game from being started twice
		bool m_hasStarted{ false };

		//replaces the current scene with the given scene
		void setScene(SceneFunction function);

		//sets the content loader to the given one
		void setContentLoader(ContentLoader& other);

		//gives access to the game context to child classes
		Context& getGameContext();

		//letterboxes the current view
		void letterbox();
	public:
		//constructor
		Game() 
			: 
			m_renderer{ m_scene }, 
			FRAMEINTV{ 1.0f / config.getConfigValue<float>(Config::configKey::FPS) },
			UPDATEINTV{ 1.0f / config.getConfigValue<float>(Config::configKey::UPS) }
		{};

		//starts the game (not done by the constructor to make sure all resources are initialized)
		void startGame();

		//ends the game
		void endGame();

		//destructor
		virtual ~Game();
	private:
		//initializes the game
		void initialize();

		//launches a threadless game loop
		void launchThreadless();

		//executes an update
		void update();

		//executes a render
		void render();
	protected:
		//functions that provide game loop access to child classes of game
		virtual void postInitialize() {};
		virtual void preUpdate(float elapsed) {};
		virtual void postUpdate(float elapsed) {};
		virtual void preRender() {};
		virtual void postRender() {};
		virtual void onGameClose() {};
	};
}

#endif