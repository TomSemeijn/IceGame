#ifndef REBOOTTRANSFORM_H
#define REBOOTTRANSFORM_H

#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"
#include "MainComponent.h"

namespace reb {

	class Animator;
	class Container;
	class ShadedSprite;

	//the transform struct holds information about the position and rotation of an object
	class Transform : public MainComponent {
		friend Animator;
		friend Container;
		friend ShadedSprite;
	private:
		//the position, scale and rotation
		Vector2 m_pos;
		float m_scale;
		float m_rotation;

		//the center to rotate around
		Vector2 m_center;
		
		//the z position (only used in rendering)
		float m_z = 0;

		//if the object is flipped or not in either axis
		bool m_flippedX = false;
		bool m_flippedY = false;

		//the local space bounds of the object
		Rect<float> m_bounds;

		//keeps the rotation value in the range of [0 => 2PI]
		void cleanRotationRange();
	public:
		//booleans for if the transform is movable and rotatable (useful for special behavior and early outs)
		const bool MOVABLE;
		const bool SCALABLE;
		const bool ROTATABLE;

		//constructor
		Transform(bool Movable = false, bool Scalable = false, bool Rotatable = false, Vector2 Pos = Vector2(), float scale = 1, float Rotation = 0);
		//copy constructor
		Transform(const Transform& other);

		//== operator
		bool operator==(const Transform& other)const;
		//!= operator
		bool operator!=(const Transform& other)const;
		//= operator
		Transform& operator=(const Transform& other);

		//returns a copy of the component
		virtual Transform* clone()const override;

		//getters
		Vector2 getPos()const;
		Vector3 getFullPos()const;
		float getZ()const;
		float getRotation()const;
		float getScale()const;
		Vector2 getCenter()const;
		const Rect<float>& getBounds()const;
		bool getFlippedX()const;
		bool getFlippedY()const;

		//modifiers
		void move(Vector2 toMove);
		void rotate(float toRotate);
		void size(float toScale);
		void toggleFlipX();
		void toggleFlipY();

		//setters
		void setPos(Vector2 newPos);
		void setRotation(float newRotation);
		void setScale(float newScale);
		void setZ(float newZ);
		void setCenter(Vector2 newCenter);
		void setBounds(Rect<float> bounds);
		void setFlipX(bool newFlip);
		void setFlipY(bool newFlip);

		//converts between worldspace and localspace
		Vector2 getLocalSpace(Vector2 worldSpace)const;
		Vector2 getWorldSpace(Vector2 localSpace)const;
		Rect<float> getLocalSpace(Rect<float> worldSpace)const;
		Rect<float> getWorldSpace(Rect<float> localSpace)const;

		//converts a normal between worldspace and localspace
		Vector2 getLocalNormal(Vector2 worldNormal)const;
		Vector2 getWorldNormal(Vector2 localNormal)const;

		//returns world-space bound corners
		Vector2 getWorldTopLeft()const;
		Vector2 getWorldTopRight()const;
		Vector2 getWorldBottomLeft()const;
		Vector2 getWorldBottomRight()const;

		//returns world-space bound edge centers
		Vector2 getWorldTopMiddle()const;
		Vector2 getWorldBottomMiddle()const;
		Vector2 getWorldLeftMiddle()const;
		Vector2 getWorldRightMiddle()const;

		//returns world-space bounds
		Rect<float> getWorldBounds()const;
		Rect<float> getWorldBounds(float localPadding)const;

		//removes the owning object, because no object can exist without a transform
		void destroy()override;
	};

}

#endif