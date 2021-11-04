#ifndef REBOOTINPUT_H
#define REBOOTINPUT_H

#pragma once
#include "Context.h"

namespace reb {

	class Game;

	//the input class handles all the window events and passes them to the first context object that handles it
	//there are separate variables for the contexts from the game, gui and scene to guarantee their priority (in that order)
	//the input class is not responsible for freeing their memory, but it is responsible for the memory of other contexts
	class Input {
		friend Game;
	private:
		//special contexts
		Context* m_gameContext = nullptr;
		Context* m_GuiContext = nullptr;
		Context* m_sceneContext = nullptr;

		//regular contexts
		std::vector<Context*> m_contexts;

		//finalizing adding contexts
		std::vector<Context*> m_toAdd_context;
		void finalAddContext();

		//finalizes removing contexts
		std::vector<Context*> m_toRemove_context;
		void finalRemoveContext();
	public:
		//updates the input
		void update(sf::Window& window, float elapsed);

		//context functions
		Context* addContext(Context* context);
		void removeContext(Context* context);

		//destructor
		~Input();
	};
}

#endif