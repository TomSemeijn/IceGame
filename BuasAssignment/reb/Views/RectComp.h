#ifndef REBOOTVIEWSRECTCOMP_H
#define REBOOTVIEWSRECTCOMP_H

#pragma once
#include "../ViewComp.h"

namespace reb {

	namespace Views {

		//adds a rectangle to the view that can be given a fillcolor, outline color and outline thickness
		class RectComp : public ViewComp {
		private:
			//color and outline settings
			Color m_fillColor;
			Color m_outlineColor;
			float m_outlineThickness;

		public:
			//constructor
			RectComp(Color fillColor, Color outlineColor = Color(), float outlineThickness = 0) : m_fillColor{ fillColor }, m_outlineColor{ outlineColor }, m_outlineThickness{ outlineThickness }{};

			//draws the rectangle to the screen
			void draw(sf::RenderTarget& window, Shader& shader, float scale)const override;

			//getters
			Color getFillColor()const;
			Color getOutlineColor()const;
			float getOutlineThickness()const;

			//setters
			void setFillColor(Color newColor);
			void setOutlineColor(Color newColor);
			void setOutlineThickness(float newThickness);
		};

	}

}

#endif