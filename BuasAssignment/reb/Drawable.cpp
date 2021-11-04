#include "Drawable.h"

namespace reb {

	//constructor
	Drawable::Drawable(Vector2 offset) : m_offset{ offset } {};

	//returns the opacity
	float Drawable::getOpacity() {
		return (float)m_color.getA() / 255.0f;
	}

	//sets the opacity
	void Drawable::setOpacity(float newOpacity) {
		m_color.setA((int)(newOpacity * 255.0f));
	}

	//returns the color
	Color Drawable::getColor() {
		return m_color;
	}

	//sets the color
	void Drawable::setColor(Color newColor) {
		m_color = newColor;
	}

	//returns the offset
	Vector2 Drawable::getOfsset() {
		return m_offset;
	}

	//sets the offset
	void Drawable::setOffset(Vector2 newOffset) {
		m_offset = newOffset;
	}

}