#include "SpriteButton.h"
#include "../Concave.h"

namespace reb {

	namespace Views {

		//constructor
		SpriteButton::SpriteButton(SpriteComp* sprite, Content* hoverSprite, Content* clickSprite, Callback onClick, Vector2 pos)
			:	View{
					Vector2(sprite->getSprite()->getArea().width, sprite->getSprite()->getArea().height), 
					new Convex(sprite->getSprite()->getConvexes().front()),
					pos
				},
				m_sprite{sprite},
				m_idleSprite{sprite->getSprite()},
				m_clickSprite{clickSprite},
				m_hoverSprite{hoverSprite},
				m_onRelease{onClick}
		{
			addComp(m_sprite);
		}

		//sets the cursor when drawing
		void SpriteButton::draw(sf::RenderTarget& window, Shader& shader, float scale)const {
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
		void SpriteButton::onClick() {
			m_sprite->setSprite(m_clickSprite);
		}

		void SpriteButton::onHeld() {
			m_sprite->setSprite(m_clickSprite);
			m_buttonHeld = true;
		}

		void SpriteButton::onRelease() {
			if (!m_mouseInside) {
				m_sprite->setSprite(m_idleSprite);
			}
			else {
				m_sprite->setSprite(m_hoverSprite);
			}
			m_buttonHeld = false;
			m_onRelease();
		}

		void SpriteButton::onMouseEnter() {
			m_mouseInside = true;
			if (!m_buttonHeld) {
				m_sprite->setSprite(m_hoverSprite);
			}
		}

		void SpriteButton::onMouseLeave() {
			m_mouseInside = false;
			if (!m_buttonHeld) {
				m_sprite->setSprite(m_idleSprite);
			}
		}

		//replaces the callback
		void SpriteButton::setOnClick(Callback onClick) {
			m_onRelease = onClick;
		}

	}

}