#ifndef REBOOTDRAWABLE_H
#define REBOOTDRAWABLE_H

#pragma once
#include "MainComponent.h"
#include <SFML/Graphics.hpp>
#include "Color.h"

namespace reb {

	class Renderer;

	//abstract base class for all main drawable components of a container
	class Drawable : public MainComponent {
	protected:
		//the offset of the sprite from the position of the object
		Vector2 m_offset;
		//the color of the drawable
		Color m_color = Color(255, 255, 255);

	public:
		//constructor
		Drawable(Vector2 offset);

		//abstract funciton for drawing
		virtual void draw(Renderer& renderer, const sf::FloatRect& viewBox, const sf::Vector2u& windowSize) = 0;

		//opacity functions
		float getOpacity();
		void setOpacity(float newOpacity);

		//color functions
		Color getColor();
		void setColor(Color newColor);

		//offset functions
		Vector2 getOfsset();
		void setOffset(Vector2 newOffset);

	};

}

#endif