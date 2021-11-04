#pragma once
#include <reb/GameObj.h>
#include <reb/ContentLoader.h>
#include "Level.h"

//the iceblock shrinks as its temperature rises and needs to be brought to the end of the level to win
class Iceblock : public reb::GameObj {
private:
	//the level the iceblock is in
	Level* m_level;

	//the current heat value
	float m_heat = 0;

protected:
	//updates the iceblock
	void lateUpdate(float elapsed)override;
public:
	//the scale to heat ratio
	const float SCALETOHEAT = -0.000001;

	//scale bounds
	const float MAXSCALE = 1.2f;
	const float BASESCALE = 1.0f;
	const float MINSCALE = 0.2f;

	//constructor
	Iceblock(Level* level, reb::ContentLoader& loader, reb::Vector2 pos);

	//returns the current heat value
	float getHeat();

	//returns the current scale position in a [0 => 2] range 
	//where [0 => 1] = [MINSCALE => BASESCALE] and [1 => 2] = [BASESCALE =>MAXSCALE]
	float getScaleRange();
};