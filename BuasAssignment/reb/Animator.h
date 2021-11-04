#ifndef REBOOTANIMATOR_H
#define REBOOTANIMATOR_H

#pragma once
#include "OwnedObject.h"
#include <chrono>
#include <unordered_set>
#include "Container.h"
#include "Content.h"

namespace reb {

	class Scene;
	class Renderer;

	//abstract base class for all animators
	class Animator : public OwnedObject<Scene> {
		friend Scene;
		friend Renderer;
	public:
		//enum for defining which part of the object should stay steady when animated
		enum SteadyAnim {
			STEADY_POS,
			STEADY_CENTER,
			STEADY_TOPLEFT,
			STEADY_TOPRIGHT,
			STEADY_BOTTOMLEFT,
			STEADY_BOTTOMRIGHT,
			STEADY_TOPMIDDLE,
			STEADY_BOTTOMMIDDLE,
			STEADY_LEFTMIDDLE,
			STEADY_RIGHTMIDDLE
		};
	private:
		//booleans for what this animator will update
		const bool UPDATESPRITES;
		const bool UPDATERIGIDBODIES;

		//which part of the object should be kept steady when animating
		const SteadyAnim STEADYTYPE;

		//the timestamp of the last update
		std::chrono::steady_clock::time_point m_lastUpdate = std::chrono::steady_clock::now();

		//updates the animator
		void updateAnim();

		//returns the steady point from the given transform
		Vector2 getSteadyPoint(Transform& trans);
	protected:
		//objects to animate
		std::unordered_set<Container*> m_objects;
		//animation frames
		std::vector<Content*> m_frames;
		//the time in seconds
		float m_time = 0;

		//sets the given frame to the given object
		void updateFrame(Container* obj, int frameIndex);
		//sets the frame to all the objects in the animator
		void updateFrame(int frameIndex);

		//updates the animation
		virtual void updateAnimation() = 0;
	public:
		//constructor
		Animator(bool updateSprites, bool updateRigidbodies, SteadyAnim steadyType = SteadyAnim::STEADY_POS) : 
			UPDATESPRITES{ updateSprites },
			UPDATERIGIDBODIES{ updateRigidbodies },
			STEADYTYPE{steadyType}
		{};

		//adds an object to be animated
		void addObject(Container* obj);
		//removes an object from the animator
		void removeObject(Container* obj);

		//removes the animator from the scene
		void destroy()override;

		virtual ~Animator() {};
	};

}

#endif