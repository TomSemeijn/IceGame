#ifndef REBOOTVIEWSRECTBUTTON_H
#define REBOOTVIEWSRECTBUTTON_H

#pragma once
#include "../View.h"
#include <functional>
#include "RectComp.h"

namespace reb {

	namespace Views {

		//a rectangular button view with a given onClick callback and a color for every button state
		class RectButton : public View {
		private:
			//typedef for click callback
			typedef std::function<void()> Callback;

			//called when the button is released
			Callback m_onRelease;

			//colors for the various click phases
			Color m_idleColor;
			Color m_hoverColor;
			Color m_clickColor;

			//the rectangle component
			RectComp* m_Rectangle;

			//true if the mouse has entered but not left
			bool m_mouseInside = false;

			//true if the button is being held
			bool m_buttonHeld = false;

			//the current cursor type
			mutable sf::Cursor::Type m_currentCursor = sf::Cursor::Arrow;

		private:
			//button events
			void onClick()override;
			void onHeld()override;
			void onRelease()override;
			void onMouseEnter()override;
			void onMouseLeave()override;
		public:
			//constructor
			RectButton(Vector2 size, RectComp* rectangle, Color hoverColor, Color clickColor, Callback onClick, Vector2 pos = Vector2());

			//sets the cursor when drawing
			virtual void draw(sf::RenderTarget& window, Shader& shader, float scale)const override;

			//replaces the callback
			void setOnClick(Callback onClick);
		};

	}

}

#endif