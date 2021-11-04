#include "WriteTextComp.h"

namespace reb {

	namespace Views {

		//draws the text to the screen
		void WriteTextComp::draw(sf::RenderTarget& window, Shader& shader, float scale)const {
			//keeps the current string
			std::string currentStr = m_text;
			//skips if the writing has already finished
			if (!m_finished) {
				//gets the current amount of characters
				int charAmount = std::min((int)currentStr.length(), (int)std::floorf(m_speed * m_time));
				//sets m_text to have that amount of characters
				m_text = "";
				if (charAmount > 0) {
					m_text.insert(m_text.begin(), currentStr.begin(), currentStr.begin() + (charAmount));
				}
				//sets finished to true if the amount fo characters is equal to the length of the written string
				if (charAmount == currentStr.length()) {
					m_finished = true;
				}
			}
			//draws the text like a normal text component would
			TextComp::draw(window, shader, scale);
			//sets the string back to the original
			m_text = currentStr;
		}

		//adds to the timer
		void WriteTextComp::update(float elapsed) {
			if (m_text != m_lastString) {
				m_time = 0;
				m_finished = false;
			}
			m_time += elapsed;
			m_lastString = m_text;
		}

		//getters
		float WriteTextComp::getSpeed() {
			return m_speed;
		}

		bool WriteTextComp::isFinished() {
			return m_finished;
		}

		//setters
		void WriteTextComp::setSpeed(float newSpeed) {
			m_speed = newSpeed;
		}

	}

}