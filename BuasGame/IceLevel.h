#pragma once
#include "Level.h"
#include <reb/Tilemap.h>
#include "Iceblock.h"

class IceLevel : public Level {
private:
	reb::Tilemap* m_terrainMap = nullptr;
	Iceblock* m_iceblock = nullptr;

	void loadContent(reb::ContentLoader& content)override;
	void setupLevel(IceGame& game, sf::RenderTarget& window, reb::Scene& scene, reb::ContentLoader& content)override;
	void setupGUI(IceGame& game, sf::RenderTarget& window, reb::GUI& gui, reb::ContentLoader& content)override;
	void registerLights(reb::Shader& shader)override;

	//adds a crystal with the given color
	void addCrystal(reb::Scene& scene, reb::ContentLoader& content, reb::Color color, reb::Vector2 pos, float rotation, bool flipX = false);
public:
	//constructor
	IceLevel() : Level(1) {};
};