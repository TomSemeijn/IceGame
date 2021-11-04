#include "Transform.h"
#include "GameObj.h"
#include "Functions.h"

namespace reb {

	//constructor
	Transform::Transform(bool Movable, bool Scalable, bool Rotatable, Vector2 Pos, float scale, float Rotation)
		: MOVABLE{ Movable },
		ROTATABLE{ Rotatable },
		SCALABLE{ Scalable },
		m_pos{ Pos },
		m_scale{ scale },
		m_rotation{ Rotation }
	{ cleanRotationRange(); }

	//copy constructor
	Transform::Transform(const Transform& other) 
		: MOVABLE{ other.MOVABLE },
		ROTATABLE{ other.ROTATABLE },
		SCALABLE{ other.SCALABLE },
		m_pos{ other.m_pos },
		m_scale{ other.m_scale },
		m_rotation{ other.m_rotation },
		m_flippedX{ other.m_flippedX },
		m_flippedY{ other.m_flippedY },
		m_bounds{other.m_bounds},
		m_z{ other.m_z }
	{ cleanRotationRange(); }

	//== operator
	bool Transform::operator==(const Transform& other)const {
		return 
			m_pos == other.m_pos && 
			m_rotation == other.m_rotation && 
			m_scale == other.m_scale && 
			m_flippedX == other.m_flippedX && 
			m_flippedY == other.m_flippedY &&
			m_bounds == other.m_bounds;
	}

	//!= operator
	bool Transform::operator!=(const Transform& other)const {
		return
			m_pos != other.m_pos ||
			m_rotation != other.m_rotation ||
			m_scale != other.m_scale ||
			m_flippedX != other.m_flippedX ||
			m_flippedY != other.m_flippedY ||
			m_bounds != other.m_bounds;
	}

	//= operator
	Transform& Transform::operator=(const Transform& other) {
		m_pos = other.m_pos;
		m_rotation = other.m_rotation;
		m_scale = other.m_scale;
		m_flippedX = other.m_flippedX;
		m_flippedY = other.m_flippedY;
		m_bounds = other.m_bounds;
		return *this;
	}

	//returns a copy of the component
	Transform* Transform::clone()const {
		return new Transform(*this);
	}

	//returns if the object is flipped in the x axis
	bool Transform::getFlippedX()const {
		return m_flippedX;
	}
	
	//returns if the object is flipped in the y axis
	bool Transform::getFlippedY()const {
		return m_flippedY;
	}

	//toggles the flip in the x axis
	void Transform::toggleFlipX() {
		m_flippedX = !m_flippedX;
	}

	//toggles the flip in the y axis
	void Transform::toggleFlipY() {
		m_flippedY = !m_flippedY;
	}

	//sets the flip in the x axis
	void Transform::setFlipX(bool newFlip) {
		m_flippedX = newFlip;
	}

	//sets the flip in the y axis
	void Transform::setFlipY(bool newFlip) {
		m_flippedY = newFlip;
	}

	//returns the current 2D position
	Vector2 Transform::getPos()const { return Vector2(m_pos.x, m_pos.y); };

	//returns the full current position (including z)
	Vector3 Transform::getFullPos()const { return Vector3(m_pos, m_z); };

	//returns the z position
	float Transform::getZ()const {
		return m_z;
	};

	//returns the current rotation
	float Transform::getRotation()const { return m_rotation; };

	//moves the transform if it's movable
	void Transform::move(Vector2 toMove) {
		if (MOVABLE) {
			m_pos += toMove;
		}
	};

	//returns the current scale
	float Transform::getScale()const {
		return m_scale;
	}

	//sets the position if movable
	void Transform::setPos(Vector2 newPos) {
		if (MOVABLE) {
			m_pos = newPos;
		}
	}

	//sets the rotation if rotatable
	void Transform::setRotation(float newRotation) {
		if (ROTATABLE) {
			m_rotation = newRotation;
			cleanRotationRange();
		}
	}

	//rotates the transform if it's rotatable
	void Transform::rotate(float toRotate) {
		if (ROTATABLE) {
			m_rotation += toRotate;
			cleanRotationRange();
		}
	};

	//sets the scale if it's scalable
	void Transform::size(float toScale) {
		if (SCALABLE) {
			m_scale = std::max(EPSILON, m_scale * toScale);
		}
	}

	//sets the scale if it's scalable
	void Transform::setScale(float newScale) {
		if (SCALABLE) {
			m_scale = std::max(EPSILON, newScale);
		}
	}

	//sets the z position
	void Transform::setZ(float newZ) { 
		m_z = newZ; 
	};

	//sets the center
	void Transform::setCenter(Vector2 newCenter) { m_center = newCenter; };
	//returns the center
	Vector2 Transform::getCenter()const { return m_center; };

	//sets the bounds
	void Transform::setBounds(Rect<float> bounds) {
		m_bounds = bounds;
	}

	//returns the bounds
	const Rect<float>& Transform::getBounds()const {
		return m_bounds;
	}

	//converts the given worldspace normal to the localspace of this transform
	Vector2 Transform::getLocalNormal(Vector2 worldNormal)const {
		//gets an origin vector
		Vector2 origin{ 0, 0 };
		//rotates
		translateRotation(worldNormal, origin, -m_rotation);
		//flips
		if (true) {
			if (m_flippedX) {
				worldNormal.x *= -1;
			}
			if (m_flippedY) {
				worldNormal.y *= -1;
			}
		}
		//returns result
		return worldNormal;
	}

	//converts the given localspace normal to worldspace
	Vector2 Transform::getWorldNormal(Vector2 localNormal)const {
		//flips
		if (true) {
			if (m_flippedX) {
				localNormal.x *= -1;
			}
			if (m_flippedY) {
				localNormal.y *= -1;
			}
		}
		//gets an origin vector
		Vector2 origin{ 0, 0 };
		//rotates
		translateRotation(localNormal, origin, m_rotation);
		//returns result
		return localNormal;
	}

	//converts the given vector from world space to the local space of this transform
	Vector2 Transform::getLocalSpace(Vector2 worldSpace)const {
		//subtracts position
		worldSpace -= m_pos;
		//rotates backwards
		translateRotation(worldSpace, m_center, -m_rotation);
		//scales
		worldSpace /= m_scale;
		//flips
		if (m_flippedX) {
			float distance = m_center.x - worldSpace.x;
			worldSpace.x += 2 * distance;
		}
		if (m_flippedY) {
			float distance = m_center.y - worldSpace.y;
			worldSpace.y += 2 * distance;
		}
		//returns result
		return worldSpace;
	}

	//converts the given vector from the local space of this transform to world space
	Vector2 Transform::getWorldSpace(Vector2 localSpace)const {
		//flips
		if (m_flippedX) {
			float distance = m_center.x - localSpace.x;
			localSpace.x += 2 * distance;
		}
		if (m_flippedY) {
			float distance = m_center.y - localSpace.y;
			localSpace.y += 2 * distance;
		}
		//scales
		localSpace *= m_scale;
		//rotates around center
		translateRotation(localSpace, m_center, m_rotation);
		//adds position
		localSpace += m_pos;
		//returns result
		return localSpace;
	}

	//returns the world-space top-left
	Vector2 Transform::getWorldTopLeft()const {
		return getWorldSpace(Vector2(m_bounds.left, m_bounds.top));
	}

	//returns the world-space top-right
	Vector2 Transform::getWorldTopRight()const {
		return getWorldSpace(Vector2(m_bounds.left + m_bounds.width, m_bounds.top));
	}

	//returns the world-space bottom-left
	Vector2 Transform::getWorldBottomLeft()const {
		return getWorldSpace(Vector2(m_bounds.left, m_bounds.top + m_bounds.height));
	}

	//returns the world-space bottom-right
	Vector2 Transform::getWorldBottomRight()const {
		return getWorldSpace(Vector2(m_bounds.left + m_bounds.width, m_bounds.top + m_bounds.height));
	}

	//returns world-space bound edge center of the top edge
	Vector2 Transform::getWorldTopMiddle()const {
		return getWorldSpace(Vector2(m_bounds.left + m_bounds.width / 2.0f, m_bounds.top));
	}

	//returns world-space bound edge center of the bottom edge
	Vector2 Transform::getWorldBottomMiddle()const {
		return getWorldSpace(Vector2(m_bounds.left + m_bounds.width / 2.0f, m_bounds.top + m_bounds.height));
	}
	
	//returns world-space bound edge center of the left edge
	Vector2 Transform::getWorldLeftMiddle()const {
		return getWorldSpace(Vector2(m_bounds.left, m_bounds.top + m_bounds.height / 2.0f));
	}

	//returns world-space bound edge center of the right edge
	Vector2 Transform::getWorldRightMiddle()const {
		return getWorldSpace(Vector2(m_bounds.left + m_bounds.width, m_bounds.top + m_bounds.height / 2.0f));
	}

	//converts between world-space and local-space
	Rect<float> Transform::getLocalSpace(Rect<float> worldSpace)const {
		Vector2 topLeft = getLocalSpace(Vector2(worldSpace.left, worldSpace.top));
		Vector2 topRight = getLocalSpace(Vector2(worldSpace.left + worldSpace.width, worldSpace.top));
		Vector2 bottomRight = getLocalSpace(Vector2(worldSpace.left + worldSpace.width, worldSpace.top + worldSpace.height));
		Vector2 bottomLeft = getLocalSpace(Vector2(worldSpace.left, worldSpace.top + worldSpace.height));
		float left = std::min(std::min(topLeft.x, topRight.x), std::min(bottomRight.x, bottomLeft.x));
		float right = std::max(std::max(topLeft.x, topRight.x), std::max(bottomRight.x, bottomLeft.x));
		float top = std::min(std::min(topLeft.y, topRight.y), std::min(bottomRight.y, bottomLeft.y));
		float bottom = std::max(std::max(topLeft.y, topRight.y), std::max(bottomRight.y, bottomLeft.y));
		return Rect<float>{ left, top, right - left, bottom - top };
	}

	Rect<float> Transform::getWorldSpace(Rect<float> localSpace)const {
		Vector2 topLeft = getWorldSpace(Vector2(localSpace.left, localSpace.top));
		Vector2 topRight = getWorldSpace(Vector2(localSpace.left + localSpace.width, localSpace.top));
		Vector2 bottomRight = getWorldSpace(Vector2(localSpace.left + localSpace.width, localSpace.top + localSpace.height));
		Vector2 bottomLeft = getWorldSpace(Vector2(localSpace.left, localSpace.top + localSpace.height));
		float left = std::min(std::min(topLeft.x, topRight.x), std::min(bottomRight.x, bottomLeft.x));
		float right = std::max(std::max(topLeft.x, topRight.x), std::max(bottomRight.x, bottomLeft.x));
		float top = std::min(std::min(topLeft.y, topRight.y), std::min(bottomRight.y, bottomLeft.y));
		float bottom = std::max(std::max(topLeft.y, topRight.y), std::max(bottomRight.y, bottomLeft.y));
		return Rect<float>{ left, top, right - left, bottom - top };
	}

	//returns world-space bounds
	Rect<float> Transform::getWorldBounds()const {
		return getWorldSpace(getBounds());
	}

	Rect<float> Transform::getWorldBounds(float localPadding)const {
		Rect<float> bounds = getBounds();
		bounds.left += localPadding;
		bounds.top += localPadding;
		bounds.width -= localPadding;
		bounds.height -= localPadding;
		return getWorldSpace(bounds);
	}

	//keeps the rotation value in the range of [0 => 2PI]
	void Transform::cleanRotationRange() {
		float twoPi = PI * 2.0f;
		while (m_rotation > twoPi) {
			m_rotation -= twoPi;
		}
		while (m_rotation < 0) {
			m_rotation += twoPi;
		}
	}

	//removes the owning object, because no object can exist without a transform
	void Transform::destroy() {
		if (hasOwner()) {
			getOwner()->destroy();
		}
	}

}