#ifndef REBOOTVIEWCOMP_H
#define REBOOTVIEWCOMP_H

#pragma once
#include <SFML/Graphics.hpp>
#include "OwnedObject.h"
#include "Shader.h"

namespace reb {

	class View;

	//can be added to a view to add behavior and/or visuals
	class ViewComp : public OwnedObject<View> {
	private:
		//the area within the owning view available for the component
		Vector2 m_areaSize;
		float m_paddingLeft = 0;
		float m_paddingRight = 0;
		float m_paddingTop = 0;
		float m_paddingBottom = 0;

		//sets the initial area to the aabb of the owner when the owner is set
		void onOwnerSet()override;

	public:
		//draw and update
		virtual void draw(sf::RenderTarget& window, Shader& shader, float scale)const = 0;
		virtual void update(float elapsed) {};

		//removes the component from the owning view
		void destroy()override;

		//padding getters
		float getPaddingLeft()const;
		float getPaddingRight()const;
		float getPaddingTop()const;
		float getPaddingBottom()const;

		//padding setters
		void setPaddingLeft(float newPadding);
		void setPaddingRight(float newPadding);
		void setPaddingTop(float newPadding);
		void setPaddingBottom(float newPadding);

		//area getters
		Rect<float> getArea()const;
		Rect<float> getScaledArea(float scale)const;

		//sets the area
		void setAreaSize(Vector2 newSize);
	};

}

#endif