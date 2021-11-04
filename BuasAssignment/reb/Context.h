#ifndef REBOOTCONTEXT_H
#define REBOOTCONTEXT_H

#pragma once
#include <map>
#include "Event.h"
#include "Functions.h"

namespace reb {

	//a context object holds and manages event objects,
	//it then takes window events and calls event functions when necessary
	class Context {
	private:
		//all the events indexed by their window event type
		std::map<sf::Event::EventType, std::vector<Event*>> m_events;

		//finalizes adding events
		std::vector<std::pair<sf::Event::EventType, Event*>> m_toAdd_events;
		void finalAddEvents();

		//finalizes removing events
		std::vector<Event*> m_toRemove_events;
		void finalRemoveEvents();

		//true if the context is enabled
		bool m_enabled;
	public:
		//constructor
		Context(bool enabled = true) : m_enabled{ enabled } { };

		//calls events when necessary
		bool callEvent(sf::Event::EventType type, sf::Event& event, float elapsed);

		//event functions
		void addEvent(sf::Event::EventType type, Event* event);
		void removeEvent(Event* event);
		void finalize();

		//returns true if the context is enabled
		bool isEnabled();

		//enable functions
		void enable();
		void disable();
		void toggle();

		//destructor
		~Context();
	};

}

#endif