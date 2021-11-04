#ifndef REBOOTVIEWSSPRITECOMP_H
#define REBOOTVIEWSSPRITECOMP_H

#pragma once
#include "../ViewComp.h"
#include "../Content.h"

namespace reb {

	namespace Views {

		//adds a sprite to the view
		class SpriteComp : public ViewComp {
		public:
			//enum for the type of scaling the sprite should do
			enum SpriteScaleType {
				SPRITE_STRETCH,
				SPRITE_LETTERBOX,
				SPRITE_NOSCALE
			};
		private:
			//the sprite
			Content* m_sprite;

			//the color of the sprite
			Color m_color;

			//the type of scaling the sprite should do
			SpriteScaleType m_scaleType;
		public:
			//constructor
			SpriteComp(Content* sprite, Color color = Color(255, 255, 255), SpriteScaleType scaleType = SpriteScaleType::SPRITE_LETTERBOX) : m_sprite(sprite), m_scaleType(scaleType), m_color{ color } {};

			//draws the sprite to the screen
			virtual void draw(sf::RenderTarget& window, Shader& shader, float scale)const override;

			//getters
			Content*const getSprite()const;
			Color getColor()const;
			SpriteScaleType getScaleType()const;

			//setters
			void setSprite(Content* newContent);
			void setColor(Color newColor);
			void setScaleType(SpriteScaleType newType);
		};

	}

}

#endif