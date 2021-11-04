#ifndef REBOOTRENDERER_H
#define REBOOTRENDERER_H

#pragma once
#include <SFML/Graphics.hpp>
#include "Scene.h"
#include "Config.h"
#include "Shader.h"
#include "GUI.h"
#include "DrawBatch.h"

namespace reb {

	extern Config config;
	class Game;

	//renders everything on screen
	class Renderer {
		friend Game;
	private:

		//debug drawing constants
		const bool DRAWRIGIDBODIES;
		const bool DRAWSPRITES;
		const bool DRAWPARTICLES;

		//the initial amount of draw layers to create (very high so if more are necessary, creating them wouldnt be where the problem lies)
		const int INITIALLAYERS = 15;

		//shaders for different render stages
		sf::Shader m_normalShader;
		sf::Shader m_infoShader;

		//holds a pointer to the scene
		Scene const * m_scene;

		//vector of drawInfo's to be drawn
		std::vector<DrawInfo> m_toDraw;

		//vector of draw layers
		std::vector<DrawLayer*> m_drawLayers;

		//creates initial draw layers
		void createInitialLayers(sf::RenderTarget& window);

	public:
		//the shader
		Shader* m_shader = new Shader();

		//constructor
		Renderer(Scene const * scene)
			: m_scene{ scene }, 
			DRAWRIGIDBODIES{config.getConfigValue<bool>(Config::DRAWRIGIDBODIES)},
			DRAWSPRITES{config.getConfigValue<bool>(Config::DRAWSPRITES)},
			DRAWPARTICLES{config.getConfigValue<bool>(Config::DRAWPARTICLES)}
		{
			//loads the shaders from the constant strings in this class, throws exception when failed
			if (!m_normalShader.loadFromMemory(NORMAL_SHADER_STR, sf::Shader::Fragment)
				|| !m_infoShader.loadFromMemory(INFO_SHADER_STR, sf::Shader::Fragment)) {
				throw std::out_of_range("Couldn't load shaders, this should be impossible since the shaders never change, but apparently it isn't so I don't know what happened to be honest");
			}
		};

		//renders
		void render(sf::RenderTarget& window, const GUI& gui);

		//draws the given sfml sprite without shading
		void draw(sf::Sprite& spr, Content* content);

		//draws the given sfml sprite with shading
		void draw(sf::Sprite& spr, Content* content, int alpha, int cells, float baseZ);

		//draws the given drawInfo
		void draw(DrawInfo& toDraw);

		//destructor
		~Renderer();

	private:
		//============================= hard-coded shader strings =======================================
		const std::string NORMAL_SHADER_STR =
"#version 120\n\
\n\
uniform sampler2D diffuse;\n\
uniform sampler2D normal;\n\
\n\
		void main() {\n\
\n\
			if (texture2D(diffuse, gl_TexCoord[0].xy).a > 0) {\n\
				vec4 normalColor = texture2D(normal, gl_TexCoord[0].xy);\n\
				gl_FragColor = vec4(normalColor.rgb, gl_Color.a);\n\
			}\n\
			else { discard; }\n\
\n\
		}";

		const std::string INFO_SHADER_STR =
"#version 120\n\
\n\
//Puts the given shading color info at the current pixel\n\
//The shading color is arranged as follows:\n\
//\n\
//	R:	alpha [1 => 255] (infinite at 0)\n\
//	G:	flip state:		~0.25 when not flipped\n\
//						~0.5 when X-flipped\n\
//						~0.75 when Y-flipped\n\
//						 1.0 when both flipped\n\
//	B:	normalRotation [0 => 2*PI] (including the horizontal and vertical flip)\n\
//	A:	cells [1 => 255]\n\
//\n\
//	If fully transparent (a = 0), there is no shading here\n\
\n\
uniform sampler2D texture;\n\
\n\
		void main() {\n\
\n\
			if (texture2D(texture, gl_TexCoord[0].xy).a > 0) {\n\
\n\
				gl_FragColor = gl_Color;\n\
\n\
			}\n\
			else { discard; }\n\
\n\
		}";
	};

}

#endif