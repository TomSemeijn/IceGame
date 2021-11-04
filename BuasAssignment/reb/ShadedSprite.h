#ifndef REBOOTSHADEDSPRITE_H
#define REBOOTSHADEDSPRITE_H

#pragma once
#include "Sprite.h"

namespace reb {

	//a sprite that is shaded using the general lightsource and normal map system
	class ShadedSprite : public Sprite {
		friend Shader;
	private:
		//the alpha value for specular lighting (default at zero for no specular)
		int m_alpha = 0;
		//the amount of cells for cell shading (default very high for no cell shading)
		int m_cells = 255;
	public:
		//constructor
		ShadedSprite(Content* source, Vector2 offset = Vector2(), RoundMode roundMode = RoundMode::MODE_NONE) : Sprite(source, offset, roundMode) {};

		//returns a copy of the component
		virtual ShadedSprite* clone()const override;

		//draws the sprite on the referenced window
		virtual void draw(Renderer& renderer, const sf::FloatRect& viewBox, const sf::Vector2u& windowSize) override;

		//gets the alpha value
		float getAlpha();
		//sets the alpha value
		void setAlpha(float newAlpha);
		//gets the cell shading level
		int getCellShadingLevel();
		//sets the cell shading level
		void setCellShadingLevel(int newCell);
	};

}

#endif