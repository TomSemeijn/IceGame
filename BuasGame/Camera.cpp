#include "Camera.h"
#include <reb/Rigidbody.h>

//constructor
Camera::Camera(sf::RenderTarget& window)
	: 
	GameObj(new reb::Transform(true, false, false)), 
	m_window{ window }
{}

//updates the camera position at the end of the update
void Camera::updateCamera(float elapsed) {
	if (m_hasBounds && m_follow != nullptr) {

		//gets transforms
		reb::Transform* myTrans = getTransform();
		reb::Transform* followTrans = m_follow->getTransform();

		//gets centers
		reb::Vector2 currentCenter = m_window.getView().getCenter();
		reb::Vector2 followCenter = followTrans->getCenter();

		reb::Vector2 mySize = m_window.getView().getSize();
		reb::Vector2 halfSize = mySize * 0.5;
		followCenter = followTrans->getWorldSpace(followCenter);

		//gets the bounds of the camera if its center would be at the center of the follow object
		reb::Rect<float> fullCenter{ followCenter.x - halfSize.x, followCenter.y - halfSize.y, mySize.x, mySize.y };
		if (fullCenter.left < m_bounds.left) {
			fullCenter.left = m_bounds.left;
		}
		if (fullCenter.getRight() > m_bounds.getRight()) {
			fullCenter.left = m_bounds.getRight() - mySize.x;
		}
		if (fullCenter.top < m_bounds.top) {
			fullCenter.top = m_bounds.top;
		}
		if (fullCenter.getBottom() > m_bounds.getBottom()) {
			fullCenter.top = m_bounds.getBottom() - mySize.y;
		}

		//sets the new center
		reb::Vector2 newCenter = fullCenter.getCenter();

		//makes the center fully divisible by the window size on each axis 
		//to avoid (most) floating point errors during fragmentation
		//reb::Vector2 size = reb::Vector2(m_window.getSize().x, m_window.getSize().y);
		//newCenter.compProduct(size);
		//newCenter.x = std::ceil(newCenter.x);
		//newCenter.y = std::ceil(newCenter.y);
		//newCenter.compDiv(size);

		//sets the new view on the window
		sf::View newView(m_window.getView());
		newView.setCenter(newCenter.getSfml());
		m_window.setView(newView);

	}
}

//sets the object to follow
void Camera::setFollow(GameObj* newFollow) {
	m_follow = newFollow;
}

//sets the bounds
void Camera::setBounds(reb::Rect<float> newBounds) {
	m_bounds = newBounds;
	m_hasBounds = true;
}