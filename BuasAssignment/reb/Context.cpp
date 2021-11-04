#include "Context.h"
#include "Functions.h"

namespace reb {

	//calls events when necessary
	bool Context::callEvent(sf::Event::EventType type, sf::Event& event, float elapsed) {
		for (auto& ev : m_events) {
			if (ev.first == type) {
				for (auto& realEvent : ev.second) {
					if (realEvent->SHOULDCALL(event)) {
						realEvent->FUNCTION(event, elapsed);
						return true;
					}
				}
			}
		}
		return false;
	}

	//adds an event
	void Context::addEvent(sf::Event::EventType type, Event* event) {
		if (event == nullptr) { return; }
		auto paired = std::make_pair(type, event);
		if (!withinVector(m_toAdd_events, paired)) {
			m_toAdd_events.push_back(paired);
		}
	}

	//removes an event
	void Context::removeEvent(Event* event) {
		if (event != nullptr && !withinVector(m_toRemove_events, event)) {
			m_toRemove_events.push_back(event);
		}
	}

	//finalizes adding events
	void Context::finalAddEvents() {
		if (!m_toAdd_events.empty()) {
			for (auto& ev : m_toAdd_events) {
				auto typeIter = m_events.find(ev.first);
				if (typeIter == m_events.end()) {
					m_events.emplace(std::make_pair(ev.first, std::vector<Event*>{}));
					typeIter = m_events.find(ev.first);
				}
				if (!withinVector(typeIter->second, ev.second)) {
					typeIter->second.push_back(ev.second);
				}
			}
			m_toAdd_events.clear();
		}
	}

	//finalizes removing events
	void Context::finalRemoveEvents() {
		if (!m_toRemove_events.empty()) {
			for (auto& eventPair : m_events) {
				for (auto& remove : m_toRemove_events) {
					auto removeIter = std::find(eventPair.second.begin(), eventPair.second.end(), remove);
					if (removeIter != eventPair.second.end()) {
						eventPair.second.erase(removeIter);
					}
				}
			}
			for (auto& remove : m_toRemove_events) {
				delete remove;
			}
			m_toRemove_events.clear();
		}
	}

	//finalizes adding and removing events
	void Context::finalize() {
		finalAddEvents();
		finalRemoveEvents();
	}

	//returns true if the context is enabled
	bool Context::isEnabled() {
		return m_enabled;
	}

	//enables the context
	void Context::enable() {
		m_enabled = true;
	}

	//disables the context
	void Context::disable() {
		m_enabled = false;
	}

	//toggles the context
	void Context::toggle() {
		m_enabled = !m_enabled;
	}

	//destructor
	Context::~Context() {
		finalAddEvents();
		for (auto& event : m_events) {
			for (auto& ev : event.second) {
				removeEvent(ev);
			}
		}
		finalRemoveEvents();
	}

}