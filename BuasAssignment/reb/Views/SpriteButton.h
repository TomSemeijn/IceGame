#ifndef REBOOTVIEWSSPRITEBUTTON_H
#define REBOOTVIEWSSPRITEBUTTON_H

#pragma once
#include "../View.h"
#include <functional>
#include "SpriteComp.h"

namespace reb {

	namespace Views {

		//a sprite button view with a given onClick callback and an image for every button state
		class SpriteButton : public View {
		private:
			//typedef for click callback
			typedef std::function<void()> Callback;

			//called when the button is released
			Callback m_onRelease;

			//colors for the various click phases
			Content* m_idleSprite;
			Content* m_hoverSprite;
			Content* m_clickSprite;

			//the rectangle component
			SpriteComp* m_sprite;

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
			SpriteButton(SpriteComp* sprite, Content* hoverSprite, Content* clickSprite, Callback onClick, Vector2 pos = Vector2());

			//sets the cursor when drawing
			virtual void draw(sf::RenderTarget& window, Shader& shader, float scale)const override;

			//replaces the callback
			void setOnClick(Callback onClick);
		};

	}

}

#endif