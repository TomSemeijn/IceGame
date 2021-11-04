#pragma once
#include "HeatObject.h"
#include <reb/ContentLoader.h>

class Penguin : public HeatObj {
private:
	//the current movement direction
	int m_currentDir = 1;

	//the last position
	reb::Vector2 m_lastPos;

	//the movement speed in world units per second
	const float MOVESPEED = 15;

	//the minimum amount moved per second to keep the same direction
	const float MINMOVE = 3;

	//the heat amount and radius
	const float HEATAMOUNT = 180;
	const float HEATRADIUS = 150;

protected:
	//makes it so it only collides with the terrain and other penguins
	bool canCollide(reb::Container* other)override;

	//gets position on early update
	void earlyUpdate(float elapsed)override;

	//moves on late update
	void lateUpdate(float elapsed)override;

public:
	//constructor
	Penguin(Level* level, reb::Scene& scene, reb::ContentLoader& content, reb::Vector2 pos);
};