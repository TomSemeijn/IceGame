#include "LoopAnimator.h"

namespace reb {

	//constructor
	LoopAnimator::LoopAnimator(
			std::vector<std::pair<Content*, float>> frames,
			bool updateSprites, 
			bool updateRigidbodies, 
			SteadyAnim steadyType
		)
		: Animator(updateSprites, updateRigidbodies, steadyType) 
	{
		//loops through all the frames
		for (auto& frame : frames) {
			//adds the content to m_frames if it wasnt in there yet and finds the index it has in m_frames
			auto contentFind = std::find(m_frames.begin(), m_frames.end(), frame.first);
			int contentIndex = -1;
			if (contentFind != m_frames.end()) {
				contentIndex = std::distance(m_frames.begin(), contentFind);
			}
			else {
				contentIndex = m_frames.size();
				m_frames.push_back(frame.first);
			}
			//adds the found index with the frame time to m_animation
			m_animation.push_back(std::make_pair(contentIndex, frame.second));
		}
	}

	//updates the animation
	void LoopAnimator::updateAnimation() {
		//updates the frame as long as the time exceeds the current frame's frame time
		while (m_time > m_animation.at(m_currentFrame).second) {
			m_time -= m_animation.at(m_currentFrame).second;
			m_currentFrame++;
			if (m_currentFrame >= m_animation.size()) { m_currentFrame = 0; }
		}
		//updates the objects if the current frame is not equal to the last frame
		if (m_currentFrame != m_lastFrame) {
			updateFrame(m_animation.at(m_currentFrame).first);
			m_lastFrame = m_currentFrame;
		}
	}

}