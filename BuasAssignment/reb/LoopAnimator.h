#ifndef REBOOTLOOPANIMATOR_H
#define REBOOTLOOPANIMATOR_H

#pragma once
#include "Animator.h"

namespace reb {

	//a simple animator that repeats the given loop of sprites
	class LoopAnimator : public Animator {
	private:
		//the frames of the animation in order with the int as the index in the frames vector and the float as the amount
		//of time the frame should be used in seconds
		std::vector<std::pair<int, float>> m_animation;

		//the current frame index
		int m_currentFrame = 0;
		//the last frame index
		int m_lastFrame = 0;
	protected:
		//updates the animation
		void updateAnimation()override;
	public:
		//constructor
		LoopAnimator(
			std::vector<std::pair<Content*, float>> frames, 
			bool updateSprites, 
			bool updateRigidbodies, 
			SteadyAnim steadyType = SteadyAnim::STEADY_POS
		);
	};

}

#endif