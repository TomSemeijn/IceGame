#include "GhostAnimator.h"

//constructor
GhostAnimator::GhostAnimator(reb::ContentLoader& content)
	: Animator(true, true, SteadyAnim::STEADY_CENTER)
{
	//adds the frames
	m_frames.push_back(content.getContent(reb::ContentIndex::GHOST_PUSH_0));
	m_frames.push_back(content.getContent(reb::ContentIndex::GHOST_PUSH_1));
	m_frames.push_back(content.getContent(reb::ContentIndex::GHOST));
	m_frames.push_back(content.getContent(reb::ContentIndex::GHOST_BLINK));

	//adds the correct frames to the pushing animation
	m_pushAnimation.emplace_back(std::make_pair(0, 0.25));
	m_pushAnimation.emplace_back(std::make_pair(1, 0.25));

	//adds the correct frames to the idle animation
	m_idleAnimation.emplace_back(std::make_pair(2, 5));
	m_idleAnimation.emplace_back(std::make_pair(3, 0.2));
}

//updates the animation
void GhostAnimator::updateAnimation() {

	//goes through all objects to animate
	for (auto& obj : m_objects) {

		//skips if not castable to the ghost class
		if (Ghost* ghost = dynamic_cast<Ghost*>(obj)) {

			//keeps the last frame index
			int lastFrame = m_currentFrame;

			//if the ghost is currently pushing
			if (ghost->pushing) {

				//if the pushing animation is playing, the time is increased and a new frame index is set if necessary
				if (m_pushingActive) {
					//changes according to time
					while (m_time >= m_pushAnimation.at(m_currentFrame).second) {
						m_time -= m_pushAnimation.at(m_currentFrame).second;
						m_currentFrame = (m_currentFrame + 1 < m_pushAnimation.size() ? m_currentFrame + 1 : 0);
					}
					//updates the frame if it has changed
					if (m_currentFrame != lastFrame) {
						updateFrame(ghost, m_pushAnimation.at(m_currentFrame).first);
					}
				}
				//if the pushing animation isnt active and enough time has passed during the idle animation to switch
				//time is reset and the pushing animation starts
				else if (m_time > MINSWITCHTIME) {
					m_pushingActive = true;
					m_time = 0;
					m_currentFrame = 0;
					updateFrame(ghost, m_pushAnimation.at(m_currentFrame).first);
				}
			}
			//if the ghost is not currently pushing
			else {
				//if the pushing animation isnt playing, 
				//the time is increased for the idle animation and a new frame index is set if necessary
				if (!m_pushingActive) {

					//changes according to time
					while (m_time >= m_idleAnimation.at(m_currentFrame).second) {
						m_time -= m_idleAnimation.at(m_currentFrame).second;
						m_currentFrame = (m_currentFrame + 1 < m_idleAnimation.size() ? m_currentFrame + 1 : 0);
					}
					//updates the frame if it has changed
					if (m_currentFrame != lastFrame) {
						updateFrame(ghost, m_idleAnimation.at(m_currentFrame).first);
					}

				}
				//if the pushing animation is active and enough time has passed during the pushing animation to switch
				//time is reset and the idle animation starts
				else if (m_time > MINSWITCHTIME) {
					m_pushingActive = false;
					m_time = 0;
					m_currentFrame = 0;
					updateFrame(ghost, m_idleAnimation.at(m_currentFrame).first);
				}
			}
		}
	}
}