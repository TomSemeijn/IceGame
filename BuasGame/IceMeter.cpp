#include "Icemeter.h"

//constructor
Icemeter::Icemeter(Iceblock* ice, float maxColorHeat)
	: 
	reb::Views::RectComp(reb::Color(), reb::Color(), 0), 
	m_ice{ ice },
	MAXCOLORHEAT{ maxColorHeat } 
{
	setFillColor(SWITCHCOLORS.front());
};

//sets the original area if it hasnt been yet, draws normally otherwise
void Icemeter::draw(sf::RenderTarget& window, reb::Shader& shader, float scale)const {
	if (!m_originalAreaSet) {
		m_originalArea = getScaledArea(scale);
		m_originalTopPadding = getPaddingTop();
		m_originalAreaSet = true;
	}
	reb::Views::RectComp::draw(window, shader, scale);
}

//updates the rectangle to match the current state of the iceblock
void Icemeter::update(float elapsed) {
	//verifies there is an iceblock
	if (m_ice == nullptr || !m_originalAreaSet) { return; }

	//gets the current ranged size value
	float size = m_ice->getScaleRange();

	//if the size is within normal bounds
	if (size <= 1.0f) {
		//gets the amount the padding can be increased
		float maxPaddingIncrease = m_originalArea.height - m_originalTopPadding;
		//sets the top padding according to this value
		setPaddingTop(m_originalTopPadding + maxPaddingIncrease * (1.0f - size));
	}
	//if the size is above normal bounds
	else {
		setPaddingTop(m_originalTopPadding);
	}

	//finds the color
	float heat = m_ice->getHeat();
	float index = (heat - MINCOLORHEAT) / (MAXCOLORHEAT - MINCOLORHEAT) * SWITCHCOLORS.size();
	if (heat < 0) {
		index = 0;
	}
	int floored = (int)std::floorf(index);
	int ceiled = (int)std::ceilf(index);

	reb::Color switchColor;
	if (ceiled >= SWITCHCOLORS.size()) {
		switchColor = SWITCHCOLORS.back();
	}
	else {
		float between = index - floored;
		reb::Color first = SWITCHCOLORS[floored];
		reb::Color second = SWITCHCOLORS[ceiled];
		switchColor = reb::Color(
			(int)((float)second.getR() * between + (float)first.getR() * (1.0f - between)),
			(int)((float)second.getG() * between + (float)first.getG() * (1.0f - between)),
			(int)((float)second.getB() * between + (float)first.getB() * (1.0f - between))
		);
	}

	//updates to the found color
	switchColor.setA(ALPHACHANNEL);
	setFillColor(switchColor);

};