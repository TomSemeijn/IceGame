#ifndef REBOOTGUI_H
#define REBOOTGUI_H

#pragma once
#include "View.h"
#include "Config.h"
#include "Context.h"

namespace reb {

	//global config object
	extern Config config;

	class Game;

	//manages and updates and draws all gui views
	class GUI {
		friend Game;
	private:
		//all views in this gui
		std::vector<View*> m_views;

		//the size of the gui
		Vector2 m_size;

		//true if the gui is currently pausing the scene
		bool m_scenePaused = false;

		//true if the gui is currently supposed to show the cursor
		bool m_showCursor = false;

		//the input context of the gui
		Context* m_guiContext = new Context();

		//manages safely removing views
		std::vector<View*> m_toRemove_views;
		void finalRemoveViews();

		//manages safely adding views
		std::vector<View*> m_toAdd_views;
		void finalAddViews();
	public:
		//constructor
		GUI();

		//draws all the views
		void draw(sf::RenderTarget& window, Shader& shader)const;
		//updates all the views
		void update(float elapsed);

		//reutrns the input context of the gui
		Context& getGuiContext();

		//adds a view
		View* addView(View* view);
		//removes a view
		void removeView(View* view);

		//returns the size of the gui
		Vector2 getSize();

		//clears everything from the gui
		void clearGUI();

		//destructor
		~GUI();
	};

}

#endif