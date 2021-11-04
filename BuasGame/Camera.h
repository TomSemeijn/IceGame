#pragma once
#include <reb/GameObj.h>
#include <reb/Transform.h>
#include <reb/Functions.h>
#include <SFML/Graphics.hpp>

//the camera follows a given object and defines the view of the window
class Camera : public reb::GameObj {
private:
	//the minimum distance from the camera center to any goal center to be considered close enough
	const float MINDISTANCE = 0.1f;

	//the window
	sf::RenderTarget& m_window;

	//the object to follow
	reb::GameObj* m_follow = nullptr;

	//the bounds of the camera
	reb::Rect<float> m_bounds{};
	bool m_hasBounds = false;

	//the current position the camera is moving towards
	reb::Vector2 m_currentMoveTo;
	bool m_isMoving = false;

public:
	//constructor
	Camera(sf::RenderTarget& window);

	//updates the camera position at the end of the update
	void updateCamera(float elapsed);

	//sets the object to follow
	void setFollow(GameObj* newFollow);

	//sets the bounds
	void setBounds(reb::Rect<float> newBounds);
};