#pragma once
#include <reb/Component.h>
#include <reb/Container.h>
#include <reb/LightSource.h>
#include <reb/Functions.h>

class LightPulseColor : public reb::Component {
private:
	bool m_hadLight = false;
	float m_time;
	std::pair<float, float> m_faseRange;
	float m_frequencyFactor;
	std::vector<reb::Color> m_colors;
	float m_midIndex;
public:
	//constructor
	LightPulseColor(float frequencyFactor, std::pair<float, float> faseRange, std::vector<reb::Color> colors);

	//updates the color of the lightsource of the owning object if both exist
	void update(float elapsed)override;

	//returns a copy of the component
	LightPulseColor* clone()const override;
};