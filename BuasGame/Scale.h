#pragma once
#include <reb/GameObj.h>
#include <reb/Transform.h>
#include <reb/ContentLoader.h>
#include <reb/Concave.h>
#include <reb/ShadedSprite.h>
#include "ContentIndex.h"

class Gate;

class Scale : public reb::GameObj {
private:
	//the maximum distance the scale can go down
	const float MAXDOWN = 20;

	//the initial position of the scale
	const reb::Vector2 INITIALPOS;

	//true if the scale is at its lowest point
	bool m_atLowest = false;
protected:
	//forces the scale to be between the initial pos and the maximum amount around it
	void lateUpdate(float elapsed)override {
		getTransform()->setPos(reb::Vector2(INITIALPOS.x, std::fmaxf(std::fminf(getTransform()->getPos().y, INITIALPOS.y + MAXDOWN), INITIALPOS.y)));
		m_atLowest = getTransform()->getPos().y >= INITIALPOS.y + MAXDOWN - reb::EPSILON;
	};
public:
	//constructor
	Scale(reb::ContentLoader& content, reb::Vector2 pos) : reb::GameObj(new reb::Transform(true, false, false, pos)), INITIALPOS{ pos } {
		//sets rigidbody
		setBody(new reb::Rigidbody(new reb::Concave(content.getContent(reb::ContentIndex::SCALE)), reb::Material(23, 0, 1, 0.9), -70, true));

		//sets sprite
		auto spr = new reb::ShadedSprite(content.getContent(reb::ContentIndex::SCALE));
		spr->setAlpha(5);
		setSprite(spr);
	};

	//returns true if the scale is at its lowest point
	bool atLowest() { return m_atLowest; };

};