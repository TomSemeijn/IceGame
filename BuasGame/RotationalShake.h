#pragma once
#include <reb/ParticleComp.h>

class RotationalShake : public reb::ParticleComp {
private:
	std::pair<float, float> m_amplitudeRange;
	std::pair<float, float> m_frequencyRange;
	float m_amplitude;
	float m_frequency;
	float m_currentOffset = 0;
	float m_time = 0;
protected:
	//despawns the owner when it goes out of bounds
	void update(float elapsed)override;

	//returns a clone of the component
	RotationalShake* clone()override;
public:
	//constructor
	RotationalShake(std::pair<float, float> amplitudeRange, std::pair<float, float> frequencyRange);
};