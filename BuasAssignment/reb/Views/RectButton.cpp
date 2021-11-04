#include "RectButton.h"

namespace reb {

	namespace Views {

		//constructor
		RectButton::RectButton(Vector2 size, RectComp* rectangle, Color hoverColor, Color clickColor, Callback onClick, Vector2 pos)
			:	View(size, nullptr, pos),
				m_onRelease{ onClick },
				m_idleColor{ rectangle->getFillColor() }, 
				m_hoverColor{ hoverColor },
				m_clickColor{ clickColor }, 
				m_Rectangle{ rectangle } 
		{
			addComp(m_Rectangle);
		};

		//sets the cursor when drawing
		void RectButton::draw(sf::RenderTarget& window, Shader& shader, float scale)const {
			if (auto cast = dynamic_cast<sf::Window*>(&window)) {
				if (m_mouseInside || m_buttonHeld) {
					if (m_currentCursor != sf::Cursor::Hand) {
						sf::Cursor myCursor{};
						if (myCursor.loadFromSystem(sf::Cursor::Hand)) {
							cast->setMouseCursor(myCursor);
							m_currentCursor = sf::Cursor::Hand;
						}
					}
				}
				else {
					if (m_currentCursor != sf::Cursor::Arrow) {
						sf::Cursor myCursor{};
						if (myCursor.loadFromSystem(sf::Cursor::Arrow)) {
							cast->setMouseCursor(myCursor);
							m_currentCursor = sf::Cursor::Arrow;
						}
					}
				}
			}
		}

		//button events
		void RectButton::onClick() {
			m_Rectangle->setFillColor(m_clickColor);
		}

		void RectButton::onHeld() {
			m_Rectangle->setFillColor(m_clickColor);
			m_buttonHeld = true;
		}

		void RectButton::onRelease() {
			if (!m_mouseInside) {
				m_Rectangle->setFillColor(m_idleColor);
			}else{
				m_Rectangle->setFillColor(m_hoverColor);
			}
			m_buttonHeld = false;
			m_onRelease();
		}

		void RectButton::onMouseEnter() {
			m_mouseInside = true;
			if (!m_buttonHeld) {
				m_Rectangle->setFillColor(m_hoverColor);
			}
		}

		void RectButton::onMouseLeave() {
			m_mouseInside = false;
			if (!m_buttonHeld) {
				m_Rectangle->setFillColor(m_idleColor);
			}
		}

		//replaces the callback
		void RectButton::setOnClick(Callback onClick) {
			m_onRelease = onClick;
		}

	}

}