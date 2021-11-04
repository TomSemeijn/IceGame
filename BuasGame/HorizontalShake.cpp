#include "HorizontalShake.h"

//constructor
HorizontalShake::HorizontalShake(std::pair<float, float> amplitudeRange, std::pair<float, float> frequencyRange)
	:
	m_amplitudeRange{ amplitudeRange },
	m_amplitude{ reb::genRandf(amplitudeRange.first, amplitudeRange.second) },

	m_frequencyRange{ frequencyRange },
	m_frequency{ reb::genRandf(frequencyRange.first, frequencyRange.second) }
{ };

//despawns the owner when it goes out of bounds
void HorizontalShake::update(float elapsed) {
	if (hasOwner()) {
		m_time += elapsed;
		float newOffset = std::sinf(m_time * m_frequency) * m_amplitude;
		float toMoveX = newOffset - m_currentOffset;
		m_currentOffset = newOffset;
		getOwner()->m_transform.move(reb::Vector2(toMoveX, 0));
	}
};

//returns a clone of the component
HorizontalShake* HorizontalShake::clone() {
	return new HorizontalShake(m_amplitudeRange, m_frequencyRange);
};