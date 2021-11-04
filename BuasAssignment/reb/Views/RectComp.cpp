#include "RectComp.h"
#include "../View.h"

namespace reb {

	namespace Views {

		//draws the rectangle to the screen
		void RectComp::draw(sf::RenderTarget& window, Shader& shader, float scale)const {
			if (hasOwner()) {
				Rect<float> realArea = getScaledArea(scale);
				sf::RectangleShape drawShape{ sf::Vector2f(realArea.width, realArea.height) };
				drawShape.setPosition(sf::Vector2f(realArea.left, realArea.top));
				drawShape.setFillColor(m_fillColor.getSfml());
				drawShape.setOutlineColor(m_outlineColor.getSfml());
				drawShape.setOutlineThickness(m_outlineThickness);
				window.draw(drawShape);
			}
		}

		//getters
		Color RectComp::getFillColor()const {
			return m_fillColor;
		}
		Color RectComp::getOutlineColor()const {
			return m_outlineColor;
		}
		float RectComp::getOutlineThickness()const {
			return m_outlineThickness;
		}

		//setters
		void RectComp::setFillColor(Color newColor) {
			m_fillColor = newColor;
		}

		void RectComp::setOutlineColor(Color newColor) {
			m_outlineColor = newColor;
		}

		void RectComp::setOutlineThickness(float newThickness) {
			m_outlineThickness = newThickness;
		}

	}

}