#include "TextComp.h"
#include <sstream>
#include "../View.h"

namespace reb {

	namespace Views {

		//draws the text to the screen
		void TextComp::draw(sf::RenderTarget& window, Shader& shader, float scale)const {
			if (hasOwner()) {
				sf::Text drawText{ m_text, m_font, (unsigned int)std::max((float)m_fontSize * scale, 0.0f) };
				drawText.setFillColor(m_color.getSfml());
				Rect<float> realArea = getScaledArea(scale);
				drawText.setPosition(Vector2(realArea.left, realArea.top).getSfml());
				//sets bold italic and underlined settings
				{
					if (m_bold && m_italic && m_underlined) {
						drawText.setStyle(sf::Text::Bold | sf::Text::Italic | sf::Text::Underlined);
					}
					else if (m_bold && m_italic) {
						drawText.setStyle(sf::Text::Bold | sf::Text::Italic);
					}
					else if (m_bold && m_underlined) {
						drawText.setStyle(sf::Text::Bold | sf::Text::Underlined);
					}
					else if (m_italic && m_underlined) {
						drawText.setStyle(sf::Text::Italic | sf::Text::Underlined);
					}
					else if (m_bold) {
						drawText.setStyle(sf::Text::Bold);
					}
					else if (m_italic) {
						drawText.setStyle(sf::Text::Italic);
					}
					else if (m_underlined) {
						drawText.setStyle(sf::Text::Underlined);
					}
				}
				//applies wordwrap if necessary
				if (m_wrap) {
					applyWordWrap(drawText, realArea);
				}//hides horizontal overflow if text isnt wrapped but overflow is hidden
				else if (m_hideOverflow) {
					hideNoWrapOverflow(drawText, realArea);
				}
				//draws the text
				window.draw(drawText);
			}
		};

		//applies wordwrap to the given text
		void TextComp::applyWordWrap(sf::Text& text, Rect<float> area)const {

			//initializes a vector of lines
			std::vector<std::string> lines;
			int currentLine = 0;

			//keeps the original string
			std::string originalString = text.getString();

			//sets up a stringstream
			std::stringstream stream(originalString);
			std::string currentWord;

			//iterates through all the words in the original string
			for (int i = 0; stream >> currentWord; i++) {
				//adds a line if necessary
				if (currentLine == lines.size()) { lines.emplace_back(""); }

				//gets the current line string
				std::string currentLineStr = lines.at(currentLine);

				//adds a string if the current line is empty
				if (currentLineStr.empty()) {
					lines.at(currentLine) += currentWord;
					continue;
				}

				//gets the current line with this word added
				currentLineStr += " " + currentWord;

				//checks if this line would exceed the area
				text.setString(currentLineStr);
				if (text.getGlobalBounds().width > area.width) {
					//adds a line with the current word if so
					lines.push_back(currentWord);
					currentLine++;
				}
				else {
					//adds the word to the line otherwise
					lines.at(currentLine) = currentLineStr;
				}
			}

			//builds the new string with line breaks
			std::string newStr = "";
			if (lines.size() > 0) {
				//sets the first line as the string
				newStr = lines.front();
				//checks if the string exceeds the height limit
				text.setString(newStr);
				if (m_hideOverflow && text.getGlobalBounds().height > area.height) { text.setString(""); return; }
				//continues if not
				if (lines.size() > 1) {
					for (auto lineIter = lines.begin() + 1; lineIter < lines.end(); lineIter++) {
						//gets a copy of the string with the current line added
						std::string copied = newStr + "\n" + *lineIter;
						//checks if the new string would exceed the height limit
						text.setString(copied);
						if (m_hideOverflow && text.getGlobalBounds().height > area.height) { break; }
						//updates the string otherwise
						newStr = copied;
					}
				}
			}
			
			//sets the string to the text
			text.setString(newStr);

		}

		//hides overflow of the given text
		void TextComp::hideNoWrapOverflow(sf::Text& text, Rect<float> area)const {
			//checks if the current string overflows in height
			if (text.getGlobalBounds().height > area.height) {
				text.setString("");
				return;
			}
			//checks if the current string overflows in width
			if (text.getGlobalBounds().width > area.width) {
				//keeps the original string
				std::string originalStr = text.getString();

				//sets up a stringstream
				std::stringstream stream(originalStr);
				std::string currentWord;
				std::string currentString = "";
				std::string lastString = "";
				bool started = false;

				//iterates through all the words in the original string
				for (int i = 0; stream >> currentWord; i++) {

					//adds the current word (with a space unless it's the first word)
					if (!started) { started = true; currentString = currentWord; }
					else { currentString += " " + currentWord; }

					//checks if the current string overflows in width
					text.setString(currentString);
					if (text.getGlobalBounds().width > area.width) {
						text.setString(lastString);
						return;
					}
					//updates the last string to the current string
					lastString = currentString;

				}
			}
		}

		//getters
		std::string TextComp::getText()const {
			return m_text;
		}

		sf::Font& TextComp::getFont() {
			return m_font;
		}

		unsigned int TextComp::getFontSize()const {
			return m_fontSize;
		}

		Color TextComp::getColor()const {
			return m_color;
		}

		bool TextComp::isWrapped()const {
			return m_wrap;
		}

		bool TextComp::hidesOverflow()const {
			return m_hideOverflow;
		}

		bool TextComp::isBold()const {
			return m_bold;
		}

		bool TextComp::isItalic()const {
			return m_italic;
		}

		bool TextComp::isUnderlined()const {
			return m_underlined;
		}

		float TextComp::getOutlineThickness()const {
			return m_outlineThickness;
		}

		Color TextComp::getOutlineColor()const {
			return m_outlineColor;
		}

		//setters
		void TextComp::setText(std::string newText) {
			m_text = newText;
		}

		void TextComp::setFont(sf::Font newFont) {
			m_font = newFont;
		}

		void TextComp::setFontSize(unsigned int newFontSize) {
			m_fontSize = newFontSize;
		}

		void TextComp::setColor(Color newColor) {
			m_color = newColor;
		}

		void TextComp::setWrapped(bool newWrapped) {
			m_wrap = newWrapped;
		}

		void TextComp::setHideOverflow(bool newHideOverflow) {
			m_hideOverflow = newHideOverflow;
		}

		void TextComp::setBold(bool newBold) {
			m_bold = newBold;
		}

		void TextComp::setItalic(bool newItalic) {
			m_italic = newItalic;
		}

		void TextComp::setUnderlined(bool newUnderlined) {
			m_underlined = newUnderlined;
		}

		void TextComp::setOutlineThickness(float newThickness) {
			m_outlineThickness = newThickness;
		}

		void TextComp::setOutlineColor(Color newColor) {
			m_outlineColor = newColor;
		}

	}

}