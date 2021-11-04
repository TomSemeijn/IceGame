#pragma once
#include <reb/Scene.h>
#include <reb/ContentLoader.h>
#include <reb/GUI.h>
#include <reb/Views/WriteTextComp.h>
#include <SFML/Graphics.hpp>
#include "IceGame.h"

class HeatHolder;
class Iceblock;

//defines a level with multiple setup functions and a restart function that avoids reloading and remanaging of content
class Level {
	friend IceGame;
	friend HeatHolder;
private:
	//all the resources that were used to construct the level
	reb::Scene* m_scene = nullptr;
	reb::GUI* m_GUI = nullptr;
	reb::ContentLoader* m_content = nullptr;
	IceGame* m_game = nullptr;
	sf::RenderTarget* m_window = nullptr;

	//all the heat objects in the level
	std::vector<HeatHolder*> m_heatObjects;

	//restarts the level
	void actuallyRestart();
protected:
	//the ambient heat of the level
	const float AMBIENTHEAT;

	//the win and lose views
	reb::View* m_winView = nullptr;
	reb::View* m_loseView = nullptr;

	//standard level GUI setup
	void setupLevelMenu(IceGame& game, reb::GUI& gui, reb::ContentLoader& content);
	void setupIceMeter(Iceblock* iceblock, float maxColorHeat, reb::GUI& gui, reb::ContentLoader& content);
	void setupWinView(IceGame& game, reb::GUI& gui, reb::ContentLoader& content, LevelIndex nextLevel);

	//abstract functions that set up the scene
	virtual void loadContent(reb::ContentLoader& content) = 0;
	virtual void setupLevel(IceGame& game, sf::RenderTarget& window, reb::Scene& scene, reb::ContentLoader& content) = 0;
	virtual void setupGUI(IceGame& game, sf::RenderTarget& window, reb::GUI& gui, reb::ContentLoader& content) = 0;
	virtual void registerLights(reb::Shader& shader) = 0;
public:
	//constructor
	Level(float ambientHeat) : AMBIENTHEAT{ ambientHeat } {};

	//sets up the level
	reb::Scene* startLevel(IceGame& game, sf::RenderTarget& window, reb::ContentLoader& content, reb::GUI& gui, reb::Shader& shader, reb::Views::WriteTextComp* progressReport = nullptr);

	//marks the level to be restarted before the next update
	void restartLevel();

	//activates the losing view
	void lose();

	//adds bounding boxes for the player around the given scene bounds
	void addPlayerBounds(reb::Scene& scene, const reb::Rect<float> sceneBounds);

	//returns the heat on the given iceblock
	float getHeat(Iceblock* block);
};