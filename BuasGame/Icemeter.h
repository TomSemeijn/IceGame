#pragma once
#include <reb/Views/RectComp.h>
#include "Iceblock.h"

//a rectangle that changes its color and size depending on the state of the given iceblock,
//its originally given area should correspond with a full meter
class Icemeter : public reb::Views::RectComp {
private:
	//the iceblock to base everything on
	Iceblock* m_ice;

	//colors to switch to
	const std::vector<reb::Color> SWITCHCOLORS{
		{reb::Color{0, 255, 255}},
		{reb::Color{20, 200, 190}},
		{reb::Color{120, 185, 155}},
		{reb::Color{255, 100, 80}},
		{reb::Color{255, 50, 0}},
	};

	//the alpha channel value of the color
	const int ALPHACHANNEL = 180;

	//colorHeat bounds
	const float MINCOLORHEAT = 0;
	const float MAXCOLORHEAT;

	//the original area
	mutable bool m_originalAreaSet = false;
	mutable reb::Rect<float> m_originalArea;

	//the original top padding
	mutable float m_originalTopPadding;
public:
	//constructor
	Icemeter(Iceblock* ice, float maxColorHeat);

	//sets the original area if it hasnt been yet, draws normally otherwise
	void draw(sf::RenderTarget& window, reb::Shader& shader, float scale)const;

	//updates the rectangle to match the current state of the iceblock
	void update(float elapsed)override;
};