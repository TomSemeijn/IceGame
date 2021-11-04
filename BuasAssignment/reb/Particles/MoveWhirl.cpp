#include "MoveWhirl.h"

namespace reb {

	namespace Particles {

		//constructor (directionRange is in degrees)
		MoveWhirl::MoveWhirl(std::pair<float, float> radiusExpansionRange, std::pair<float, float> velocityRange)
			:
			m_radiusExpansionRange{ radiusExpansionRange },
			m_velocityRange{ velocityRange }
		{
			//gets a random value wihtin each range
			m_expansion = genRandf(m_radiusExpansionRange.first, m_radiusExpansionRange.second);
			m_velocity = genRandf(m_velocityRange.first, m_velocityRange.second);
		}

		//sets the initial position when the owning particle is set
		void MoveWhirl::onOwnerSet() {
			m_initialPos = getOwner()->m_transform.getPos() - Vector2(m_radius, m_radius);
		}

		//updates the component to allow behavior to be added to the owning particle
		void MoveWhirl::update(float elapsed) {
			//verifies there is an owner
			if (hasOwner()) {
				//gets the owner and its time
				auto owner = getOwner();
				float time = owner->getTime();
				//expands the radius
				m_radius += m_expansion * elapsed;
				//gets the current local position
				Vector2 localPos{};
				float factor = time * m_velocity;
				localPos.x = std::cosf(factor) * m_radius;
				localPos.y = std::sinf(factor) * m_radius;
				//moves to the new positon
				getOwner()->m_transform.move(m_initialPos + localPos - owner->m_transform.getPos());
			}
		}

		//returns a clone of the component
		ParticleComp* MoveWhirl::clone() {
			return new MoveWhirl(m_radiusExpansionRange, m_velocityRange);
		}

	}

}