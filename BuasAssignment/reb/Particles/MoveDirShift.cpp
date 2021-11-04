#include "MoveDirShift.h"

namespace reb {

	namespace Particles {

		//constructor (directionRange is in degrees)
		MoveDirShift::MoveDirShift(std::pair<float, float> initialDirRange, std::pair<float, float> shiftRange, std::pair<float, float> shiftTimeRange, std::pair<float, float> velocityRange)
			:
			m_initialDirRange{ initialDirRange },
			m_shiftRange{ shiftRange },
			m_velocityRange{ velocityRange },
			m_shiftTimeRange{ shiftTimeRange },
			m_currentDir{ genRandf(initialDirRange.first, initialDirRange.second) },
			m_velocity{ genRandf(velocityRange.first, velocityRange.second) },
			m_currentShiftTime{ genRandf(m_shiftTimeRange.first, m_shiftRange.second) }
		{}

		//updates the component to allow behavior to be added to the owning particle
		void MoveDirShift::update(float elapsed) {
			//verifies the component has an owner
			if (hasOwner()) {
				//increments currentTime
				m_currentTime += elapsed;

				//shifts direction as much as necessary
				while (m_currentTime > m_currentShiftTime) {
					//gets a current shift in direction
					float dirShift = genRandf(m_shiftRange.first, m_shiftRange.second);
					//shifts the direction
					m_currentDir += dirShift;
					//decrements currentTime
					m_currentTime -= elapsed;
					//regens currentShiftTime
					m_currentShiftTime = genRandf(m_shiftTimeRange.first, m_shiftRange.second);
				}

				//gets the current move vector
				Vector2 moveVec = Vector2(std::cosf(m_currentDir), std::sinf(m_currentDir)).setLength(m_velocity);
				//moves
				getOwner()->m_transform.move(moveVec * elapsed);
			}
		}

		//returns a clone of the component
		ParticleComp* MoveDirShift::clone() {
			return new MoveDirShift(m_initialDirRange, m_shiftRange, m_shiftTimeRange, m_velocityRange);
		}

	}

}