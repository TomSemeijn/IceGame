#ifndef REBOOTVIEWSWRITETEXTCOMP_H
#define REBOOTVIEWSWRITETEXTCOMP_H

#pragma once
#include "TextComp.h"

namespace reb {

	namespace Views {

		//adds text to the view, but the text writes itself down with a given amount of characters per second instead
		//of appearing all at once
		class WriteTextComp : public TextComp {
		private:
			//writing speed in characters per second
			float m_speed;

			//the current amount of time this string has been being written
			float m_time;

			//true if the full string is now on screen
			mutable bool m_finished = false;

			//the last string that was being written
			std::string m_lastString;

		public:
			//constructor
			WriteTextComp(float writeSpeed, sf::Font font, unsigned int fontSize, bool wrap = true, Color color = Color(255, 255, 255), std::string initialText = "") : TextComp(font, fontSize, wrap, color, initialText), m_speed{ writeSpeed }, m_lastString{ initialText }{}

			//draws the text to the screen
			virtual void draw(sf::RenderTarget& window, Shader& shader, float scale)const override;

			//adds to the timer
			virtual void update(float elapsed)override;

			//getters
			float getSpeed();
			bool isFinished();

			//setters
			void setSpeed(float newSpeed);
		};

	}

}

#endif