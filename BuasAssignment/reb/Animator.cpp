#include "Animator.h"
#include "Scene.h"
#include "Sprite.h"

namespace reb {

	//adds an object to be animated
	void Animator::addObject(Container* obj) {
		//verifies the object isnt a nullptr
		if (obj != nullptr) {
			//adds the object to the set
			m_objects.insert(obj);
		}
	}

	//removes an object from the animator if it has it
	void Animator::removeObject(Container* obj) {
		m_objects.erase(obj);
	}

	//returns the steady point from the given transform
	Vector2 Animator::getSteadyPoint(Transform& trans) {
		Vector2 steadyPoint;
		switch (STEADYTYPE) {
		case SteadyAnim::STEADY_POS:
			steadyPoint = trans.getPos();
			break;
		case SteadyAnim::STEADY_CENTER:
			steadyPoint = trans.getCenter();
			break;
		case SteadyAnim::STEADY_TOPLEFT:
			steadyPoint = trans.getWorldTopLeft();
			break;
		case SteadyAnim::STEADY_TOPRIGHT:
			steadyPoint = trans.getWorldTopRight();
			break;
		case SteadyAnim::STEADY_BOTTOMRIGHT:
			steadyPoint = trans.getWorldBottomRight();
			break;
		case SteadyAnim::STEADY_BOTTOMLEFT:
			steadyPoint = trans.getWorldBottomLeft();
			break;
		case SteadyAnim::STEADY_BOTTOMMIDDLE:
			steadyPoint = trans.getWorldBottomMiddle();
			break;
		case SteadyAnim::STEADY_TOPMIDDLE:
			steadyPoint = trans.getWorldTopMiddle();
			break;
		case SteadyAnim::STEADY_LEFTMIDDLE:
			steadyPoint = trans.getWorldLeftMiddle();
			break;
		case SteadyAnim::STEADY_RIGHTMIDDLE:
			steadyPoint = trans.getWorldRightMiddle();
			break;
		default:
			throw std::out_of_range("Animator has invalid steady point enum value (this message should be impossible to get)");
			break;
		}
		return steadyPoint;
	}

	//sets the given frame to the given object
	void Animator::updateFrame(Container* obj, int frameIndex) {
		//removes if object if it is nullptr
		if (obj == nullptr) {
			removeObject(obj); 
			return;
		}
		//gets the point of the object that should remain steady
		Vector2 steadyPoint = getSteadyPoint(*obj->getTransform());

		//updates the rigidbody if it should and if there is one
		if (UPDATERIGIDBODIES) {
			Rigidbody * const body = obj->getBody();
			if (body != nullptr) {
				body->animateShape(m_frames.at(frameIndex));
			}
		}
		//updates the sprite if it should and if there is one
		if (UPDATESPRITES) {
			Drawable * const sprite = obj->getSprite();
			if (sprite != nullptr) {
				if (auto cast = dynamic_cast<Sprite*>(sprite)) {
					cast->setContent(m_frames.at(frameIndex));
				}
			}
		}
		//gets the new position of the steady point
		Vector2 newPoint = getSteadyPoint(*obj->getTransform());

		//moves the object so the steady point stays in the same spot
		obj->getTransform()->m_pos += (steadyPoint - newPoint);
	}

	//sets the frame to all the objects in the animator
	void Animator::updateFrame(int frameIndex) {
		for (auto& obj : m_objects) {
			updateFrame(obj, frameIndex);
		}
	}

	//updates the animator
	void Animator::updateAnim() {
		auto now = std::chrono::steady_clock::now();
		m_time += std::chrono::duration<float>(now - m_lastUpdate).count();
		m_lastUpdate = now;
		updateAnimation();
	};

	//removes the animator from the scene
	void Animator::destroy() {
		if (hasOwner()) {
			getOwner()->removeAnimator(this);
		}
	}

}