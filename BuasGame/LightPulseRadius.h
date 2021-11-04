#pragma once
#include <reb/Component.h>
#include <reb/Container.h>
#include <reb/Functions.h>

template <typename LightType>
class LightPulseComp : public reb::Component {
private:
	float m_radiusAmplitudePercentage;
	float m_radiusFrequencyFactor;
	float m_intensityAmplitudePercentage;
	float m_intensityFrequencyFactor;
	std::pair<float, float> m_radiusFaseRange;
	std::pair<float, float> m_intensityFaseRange;
	float m_originalRadius;
	float m_originalIntensity;
	bool m_radiusSet = false;
	float m_radiusTime = 0;
	float m_intensityTime = 0;
protected:
	//gets called every update
	void update(float elapsed)override {

		//verifies there is an owner
		if (hasOwner()) {
			//increments time
			m_radiusTime += elapsed + reb::genRandf(m_radiusFaseRange.first, m_radiusFaseRange.second);
			m_intensityTime += elapsed + reb::genRandf(m_intensityFaseRange.first, m_intensityFaseRange.second);

			//tries to get the light
			if (LightType* light = dynamic_cast<LightType*>(getOwner()->getLight())) {

				if (!m_radiusSet) {
					m_radiusSet = true;
					m_originalRadius = light->getRadius();
					m_originalIntensity = light->getIntensity();
				}

				//sets the radius
				light->setRadius(m_radiusAmplitudePercentage * m_originalRadius * std::sinf(m_radiusTime * m_radiusFrequencyFactor) + m_originalRadius);
				light->setIntensity(m_intensityAmplitudePercentage * m_originalIntensity * std::sinf(m_intensityTime * m_intensityFrequencyFactor) + m_originalIntensity);
				
			}
		}

	}

	//allows child classes to react when the owner is set
	void onOwnerSet()override {
		//gets owner
		auto owner = getOwner();

		//tries to get light
		if (LightType* light = dynamic_cast<LightType*>(owner->getLight())) {
			m_radiusSet = true;
			m_originalRadius = light->getRadius();
			m_originalIntensity = light->getIntensity();
		}
	}

public:
	//constructor
	LightPulseComp(
		float radiusAmplitudePercentage, 
		float radiusFrequencyFactor, 
		std::pair<float, float> radiusFaseRange,
		float intensityAmplitudePercentage, 
		float intensityFrequencyFactor,
		std::pair<float, float> intensityFaseRange
	)
		:	m_radiusAmplitudePercentage{ radiusAmplitudePercentage }, 
			m_radiusFrequencyFactor{ radiusFrequencyFactor }, 
			m_intensityAmplitudePercentage{ intensityAmplitudePercentage }, 
			m_intensityFrequencyFactor{ intensityFrequencyFactor },
			m_radiusFaseRange{ radiusFaseRange },
			m_intensityFaseRange{ intensityFaseRange }
	{};

	//returns a copy of the component
	MainComponent* clone()const override { return new LightPulseComp<LightType>(*this); };

};