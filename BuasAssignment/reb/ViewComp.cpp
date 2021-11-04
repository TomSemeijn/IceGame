#include "ViewComp.h"
#include "View.h"

namespace reb {

	//removes the component from the owning view
	void ViewComp::destroy() {
		if (hasOwner()) {
			getOwner()->removeComp(this);
		}
	}

	//sets the initial area to the aabb of the owner when the owner is set
	void ViewComp::onOwnerSet() {
		Rect<float> aabb = getOwner()->getAABB();
		m_areaSize = Vector2(aabb.width, aabb.height);
	}

	//returns the local space area of the component
	Rect<float> ViewComp::getArea()const {
		Vector2 ownerPos{};
		if (hasOwner()) {
			ownerPos = getOwner()->getPos();
		}
		return Rect<float>(
			ownerPos.x + m_paddingLeft, 
			ownerPos.y + m_paddingTop, 
			m_areaSize.x - m_paddingLeft - m_paddingRight, 
			m_areaSize.y - m_paddingTop - m_paddingBottom
		);
	}

	//returns the scaled area of the component
	Rect<float> ViewComp::getScaledArea(float scale)const {
		Vector2 ownerPos{};
		if (hasOwner()) {
			ownerPos = getOwner()->getPos();
		}
		return Rect<float>(
			ownerPos.x + m_paddingLeft * scale, 
			ownerPos.y + m_paddingTop * scale, 
			m_areaSize.x * scale - m_paddingLeft * scale - m_paddingRight * scale,
			m_areaSize.y * scale - m_paddingTop * scale - m_paddingBottom * scale
		);
	}

	//padding getters
	float ViewComp::getPaddingLeft()const {
		return m_paddingLeft;
	}

	float ViewComp::getPaddingRight()const {
		return m_paddingRight;
	}

	float ViewComp::getPaddingTop()const {
		return m_paddingTop;
	}

	float ViewComp::getPaddingBottom()const {
		return m_paddingBottom;
	}

	//padding setters
	void ViewComp::setPaddingLeft(float newPadding) {
		m_paddingLeft = newPadding;
	}

	void ViewComp::setPaddingRight(float newPadding) {
		m_paddingRight = newPadding;
	}

	void ViewComp::setPaddingTop(float newPadding) {
		m_paddingTop = newPadding;
	}

	void ViewComp::setPaddingBottom(float newPadding) {
		m_paddingBottom = newPadding;
	}

	//sets the local-space area
	void ViewComp::setAreaSize(Vector2 newSize) {
		m_areaSize = newSize;
	}

}