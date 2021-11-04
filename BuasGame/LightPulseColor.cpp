#include "LightPulseColor.h"

//constructor
LightPulseColor::LightPulseColor(float frequencyFactor, std::pair<float, float> faseRange, std::vector<reb::Color> colors) : m_frequencyFactor{ frequencyFactor }, m_faseRange{ faseRange }, m_colors{ colors }, m_midIndex{ (float)m_colors.size() / 2.0f }{};

//updates the color of the lightsource of the owning object if both exist
void LightPulseColor::update(float elapsed) {
	if (hasOwner()) {
		if (auto light = getOwner()->getLight()) {
			m_time += elapsed + reb::genRandf(m_faseRange.first, m_faseRange.second);
			float currentPlace = m_midIndex * std::sinf(m_time * m_frequencyFactor) + m_midIndex;
			int lowerIndex = (int)std::floorf(currentPlace);
			int upperIndex = (int)std::ceilf(currentPlace);
			float midPos = currentPlace - lowerIndex;
			if (upperIndex == m_colors.size()) {
				light->setColor(m_colors.back());
			}
			else {
				reb::Color lower = m_colors[upperIndex];
				reb::Color upper = m_colors[lowerIndex];
				int r = (int)((float)lower.getR() * midPos + (float)upper.getR() * (1.0f - midPos));
				int g = (int)((float)lower.getG() * midPos + (float)upper.getG() * (1.0f - midPos));
				int b = (int)((float)lower.getB() * midPos + (float)upper.getB() * (1.0f - midPos));
				light->setColor(reb::Color(r, g, b));
			}
		}
	}
}

//returns a copy of the component
LightPulseColor* LightPulseColor::clone()const { return new LightPulseColor(*this); };