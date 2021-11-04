#pragma once
#include <reb/Game.h>
#include <thread>
#include <reb/Scene.h>
#include <reb/Vector2.h>
#include <reb/ContentLoader.h>
#include <reb/Views/WriteTextComp.h>

#include "LevelIndex.h"

using reb::writeLn;

namespace reb {
	extern Config config;
}

class Level;

class IceGame : public reb::Game {
private:
	reb::Scene* setupLoadingScreen(reb::ContentLoader& newLoader, reb::GUI& newGui, reb::Shader& newShader);
	reb::Views::WriteTextComp* m_setupTextComp = nullptr;

	std::map<LevelIndex, Level*> m_levels;
	LevelIndex m_currentLevel = NOLEVEL_LOADING;

	bool m_currentFullScreen = reb::config.getConfigValue<bool>(reb::Config::FULLSCREEN);
	bool m_updateFullScreen = false;

	sf::Image m_icon;

	std::thread m_setupThread;
	bool m_setupRunning = false;
	std::string setupInfo;
	bool m_restart = false;

	//saves the current fullscreen setting to the config file
	void saveFullScrConfig();
protected:
	//game loop functions
	void postInitialize()override;
	void preUpdate(float elapsed)override;
	void postUpdate(float elapsed)override;
	void preRender()override;
	void postRender()override;
	void onGameClose()override;
public:
	//switches to a new level
	void setLevel(LevelIndex level);

	//makes the current level restart
	void restartCurrentLevel();

	float getFps() { return m_fps; }

	//destructor
	~IceGame();
};