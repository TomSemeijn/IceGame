#pragma once
#include <reb/Animator.h>
#include <reb/ContentLoader.h>
#include "Ghost.h"
#include "ContentIndex.h"

//animator specifically for the player ghost object to switch between animations depending on wether or not the
//player is pushing something
class GhostAnimator : public reb::Animator {
private:
	//the minimum amount of time between animations
	const float MINSWITCHTIME = 0.2;

	//the animation tracks
	std::vector<std::pair<int, float>> m_idleAnimation{};
	std::vector<std::pair<int, float>> m_pushAnimation{};

	//true if the pushing animation is playing
	bool m_pushingActive = false;

	//the index of the currently displayed frame
	int m_currentFrame = 0;
protected:
	//updates the animation
	void updateAnimation()override;

public:
	//constructor
	GhostAnimator(reb::ContentLoader& content);

};