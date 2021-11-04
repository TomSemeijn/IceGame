#ifndef REBOOTEVENT_H
#define REBOOTEVENT_H

#pragma once
#include <functional>
#include <SFML/Window.hpp>

namespace reb {

	//calls the given function if the given shouldcall function returns true
	class Event {
	public:
		//returns true if the event should be called with the given event
		const std::function<bool(sf::Event&)> SHOULDCALL;

		//the callback of this event
		const std::function<void(sf::Event&, float)> FUNCTION;


		//constructor
		Event(std::function<bool(sf::Event&)> shouldCall, std::function<void(sf::Event&, float)> function) : SHOULDCALL{ shouldCall }, FUNCTION{ function }{};

	};

}

#endif