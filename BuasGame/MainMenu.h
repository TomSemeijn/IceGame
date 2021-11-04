#pragma once
#include "Level.h"

class MainMenu : public Level {
private:
	void loadContent(reb::ContentLoader& content)override;
	void setupLevel(IceGame& game, sf::RenderTarget& window, reb::Scene& scene, reb::ContentLoader& content)override;
	void setupGUI(IceGame& game, sf::RenderTarget& window, reb::GUI& gui, reb::ContentLoader& content)override;
	void registerLights(reb::Shader& shader)override;

	//adds a levelview with the given thumbnail and levelIndex
	void addLevelView(reb::View* mainView, IceGame& game, reb::GUI& gui, reb::ContentLoader& content, reb::ContentIndex thumbnailIndex, LevelIndex level, reb::Vector2 viewPos);

public:
	//constructor
	MainMenu() : Level(0) {};
};