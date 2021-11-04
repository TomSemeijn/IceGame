#ifndef REBOOTSPRITE_H
#define REBOOTSPRITE_H

#pragma once
#include <memory>
#include "Drawable.h"
#include "Content.h"
#include "Transform.h"

namespace reb {

	class Shader;

	//a sprite that is not shaded at all, only the diffuse image is used
	class Sprite : public Drawable {
	protected:
		//the content the sprite is based on
		Content* m_source;

		//updates the bounds of the owner if that's the job of the sprite
		virtual void updateBounds();

		//gets called when the owner of the component is set
		virtual void onOwnerSet()override;

	public:
		//if the sprite should be drawn with floored coords or not (draws with sub-pixels if not)
		enum RoundMode { MODE_FLOOR, MODE_CEIL, MODE_ROUND, MODE_NONE };
		const RoundMode ROUNDMODE;

		//constructor
		Sprite(Content* source, Vector2 offset = Vector2(), RoundMode drawFloored = RoundMode::MODE_NONE)
			:	Drawable{offset},
				m_source { source },
				ROUNDMODE{drawFloored}
		{};

		//returns a copy of the component
		virtual Sprite* clone()const override;

		//draws the sprite on the referenced window
		virtual void draw(Renderer& renderer, const sf::FloatRect& viewBox, const sf::Vector2u& windowSize);

		//updates the content the sprite is based on
		void setContent(Content* newContent);
		const Content* getContent();

		//removes the sprite from its owning object
		void destroy()override;

		//virtual destructor
		virtual ~Sprite() {};
	};

}

#endif