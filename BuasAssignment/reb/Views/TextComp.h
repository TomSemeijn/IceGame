#ifndef REBOOTVIEWSTEXTCOMP_H
#define REBOOTVIEWSTEXTCOMP_H

#pragma once
#include "../ViewComp.h"

namespace reb {

	namespace Views {

		//adds text to a view
		class TextComp : public ViewComp {
		protected:
			//the current text and its basic settings
			mutable std::string m_text;
			sf::Font m_font;
			unsigned int m_fontSize;
			Color m_color;

			//if text should be wrapped or not
			bool m_wrap;

			//if wrapped text should be hidden or not
			bool m_hideOverflow = true;

			//bold, italic and underlined
			bool m_bold = false;
			bool m_italic = false;
			bool m_underlined = false;

			//outline information
			float m_outlineThickness = 0;
			Color m_outlineColor = Color(255, 255, 255);

			//applies wordwrap to the given text
			void applyWordWrap(sf::Text& text, Rect<float> area)const;

			//hides overflow of the given text (only called if wordwrap isnt applied)
			void hideNoWrapOverflow(sf::Text& text, Rect<float> area)const;

		public:
			//constructor
			TextComp(sf::Font font, unsigned int fontSize, bool wrap = true, Color color = Color(255, 255, 255), std::string initialText = "") : m_font{ font }, m_fontSize{ fontSize }, m_wrap{ wrap }, m_text{ initialText }, m_color{ color } {};

			//draws the text to the screen
			virtual void draw(sf::RenderTarget& window, Shader& shader, float scale)const override;

			//getters
			std::string getText()const;
			sf::Font& getFont();
			unsigned int getFontSize()const;
			Color getColor()const;
			bool isWrapped()const;
			bool hidesOverflow()const;
			bool isBold()const;
			bool isItalic()const;
			bool isUnderlined()const;
			float getOutlineThickness()const;
			Color getOutlineColor()const;
			
			//setters
			void setText(std::string newText);
			void setFont(sf::Font newFont);
			void setFontSize(unsigned int newFontSize);
			void setColor(Color newColor);
			void setWrapped(bool newWrapped);
			void setHideOverflow(bool newHideOverflow);
			void setBold(bool newBold);
			void setItalic(bool newItalic);
			void setUnderlined(bool newUnderlined);
			void setOutlineThickness(float newThickness);
			void setOutlineColor(Color newColor);

		};

	}

}

#endif