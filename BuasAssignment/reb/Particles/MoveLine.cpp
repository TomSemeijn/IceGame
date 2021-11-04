#include "MoveLine.h"

namespace reb {

	namespace Particles {

		//constructor (directionRange is in degrees)
		MoveLine::MoveLine(std::pair<float, float> directionRange, std::pair<float, float> velocityRange)
			:
			m_directionRange{ directionRange },
			m_velocityRange{ velocityRange }
		{
			//gets the random direction in degrees and the velocity in world units per second
			float finalDirection = getRadians(genRandf(m_directionRange.first, m_directionRange.second));
			float velocity = genRandf(m_velocityRange.first, m_velocityRange.second);

			//uses the found direction to create the direction vector
			m_moveVec = Vector2(std::cosf(finalDirection), std::sinf(finalDirection)).setLength(velocity);
		}

		//updates the component to allow behavior to be added to the owning particle
		void MoveLine::update(float elapsed) {
			//verifies there is an owner
			if (hasOwner()) {
				//moves the particle
				getOwner()->m_transform.move(m_moveVec * elapsed);
			}
		}

		//returns a local aabb of the component (returns an empty aabb by default)
		Rect<float> MoveLine::getLocalAABB(float maxLifetime) {
			//gets the aabb's for the closest values to the most extreme directions within the range
			float extreme[4] = { 0, 90, 180, 270 };
			Rect<float> aabb{};
			float minDir = std::min(m_directionRange.first, m_directionRange.second);
			float maxDir = std::max(m_directionRange.first, m_directionRange.second);
			while (minDir < 0 || maxDir < 0) {
				minDir += 360;
				maxDir += 360;
			}
			std::vector<float> extremes{};
			extremes.push_back(minDir);
			extremes.push_back(maxDir);
			for (float& ex : extreme) {
				if (ex > minDir && ex < maxDir) {
					extremes.push_back(ex);
				}
			}
			for (float& ex : extremes) {
				float currentDir = ex;
				currentDir = std::min(currentDir, maxDir);
				currentDir = std::max(currentDir, minDir);
				Vector2 extremePos{ std::cosf(currentDir), std::sinf(currentDir) };
				extremePos.setLength(m_velocityRange.second * maxLifetime);
				float left = aabb.left;
				float right = aabb.getRight();
				float top = aabb.top;
				float bottom = aabb.getBottom();
				left = std::min(left, extremePos.x);
				right = std::max(right, extremePos.x);
				top = std::min(top, extremePos.y);
				bottom = std::max(bottom, extremePos.y);
				aabb = Rect<float>(left, top, right - left, bottom - top);
			}
			return aabb;
		}

		//returns a clone of the component
		ParticleComp* MoveLine::clone() {
			return new MoveLine(m_directionRange, m_velocityRange);
		}

	}

}