#pragma once
#include <reb/GameObj.h>
#include <reb/Rigidbody.h>
#include <reb/Vector2.h>
#include <reb/ContentLoader.h>
#include "LayerIndex.h"
#include "Camera.h"

//the ghost is the player, it can fly in any direction and move through all rigidbodies besides those on the ice layer
class Ghost : public reb::GameObj {
private:
	sf::RenderTarget& m_window;
	reb::Vector2 m_realPos;
	Camera* m_camera;
public:
	//constructor
	Ghost(reb::Scene& scene, reb::ContentLoader& content, reb::Vector2 pos, sf::RenderTarget& window, reb::Rect<float> sceneBounds);

	//booleans for moving and animation
	bool holdLeft = false;
	bool holdRight = false;
	bool holdUp = false;
	bool holdDown = false;

	//the speed of the ghost in world units per second
	const float speed = 100;

	//used by the animator to know when to play the pushing animation
	bool pushing = false;
protected:

	//sets the position back to normal before any real updating takes place
	void earlyUpdate(float elapsed)override;

	//updates the movement and pushing status
	void update(float elapsed)override;

	//updates the camera
	void lateUpdate(float elapsed)override;

	//makes the ghost only collide with ice
	bool canCollide(Container* other)override;

	//reacts when the ghost is pushing something
	void onTouch(Container* other, reb::Vector2 contactPoint, reb::Vector2 contactNormal)override;
};