#pragma once
#include <reb/GameObj.h>
#include "Scale.h"
#include "LayerIndex.h"

class Gate : public reb::GameObj {
private:
	const reb::Vector2 INITIALPOS;
	const float GRAVITY = 150;
	const float UPFACTOR = 0.75;

	float m_height;
	Scale* m_scale;

protected:

	//changes the gravity of the gate depending on the state of the scale
	void earlyUpdate(float elapsed)override {
		if (m_scale != nullptr && m_scale->atLowest()) {
			getBody()->setGravity(-GRAVITY);
		}
		else {
			getBody()->setGravity(GRAVITY);
		}
	}

	//forces the gate to stay within its initial y-position and its own height higher
	void lateUpdate(float elapsed)override {
		getTransform()->setPos(reb::Vector2(INITIALPOS.x, std::fminf(std::fmaxf(getTransform()->getPos().y, INITIALPOS.y - m_height * UPFACTOR), INITIALPOS.y)));
	};

public:
	//constructor
	Gate(reb::ContentLoader& content, Scale* scale, reb::Vector2 pos)
		: reb::GameObj(new reb::Transform(true, false, false)), INITIALPOS{ pos }, m_scale{ scale }{

		//sets rigidbody
		setBody(new reb::Rigidbody(new reb::Concave(content.getContent(reb::ContentIndex::GATE)), reb::Material(10, 0, 1, 0.9), GRAVITY, true));

		//sets sprite
		setSprite(new reb::ShadedSprite(content.getContent(reb::ContentIndex::GATE)));

		//sets height
		m_height = getBody()->getShape()->getAABB(*getTransform()).height;

	};

	//collides with anything but terrain
	bool canCollide(reb::Container* other)override {
		return other->getLayer() != reb::LayerIndex::TERRAIN && other->getLayer() != reb::LayerIndex::ICETERRAIN;
	}

};