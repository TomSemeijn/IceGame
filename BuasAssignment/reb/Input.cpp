#include "Input.h"
#include "Functions.h"

namespace reb{

	//updates the input
	void Input::update(sf::Window& window, float elapsed) {
		finalAddContext();
		finalRemoveContext();

		if (m_gameContext != nullptr) {
			m_gameContext->finalize();
		}
		if (m_GuiContext != nullptr) {
			m_GuiContext->finalize();
		}
		if (m_sceneContext != nullptr) {
			m_sceneContext->finalize();
		}
		for (auto& context : m_contexts) {
			context->finalize();
		}
		
		sf::Event ev;
		while (window.pollEvent(ev)) {
			if (m_gameContext != nullptr && m_gameContext->isEnabled() && m_gameContext->callEvent(ev.type, ev, elapsed)) {
				continue;
			}
			if (m_GuiContext != nullptr && m_GuiContext->isEnabled() && m_GuiContext->callEvent(ev.type, ev, elapsed)) {
				continue;
			}
			if (m_sceneContext != nullptr && m_sceneContext->isEnabled() && m_sceneContext->callEvent(ev.type, ev, elapsed)) {
				continue;
			}
			for (auto& context : m_contexts) {
				if (context->isEnabled() && context->callEvent(ev.type, ev, elapsed)) {
					break;
				}
			}
		}
	}

	//adds a context object to the input
	Context* Input::addContext(Context* context) {
		if (context != nullptr && !withinVector(m_toAdd_context, context)) {
			m_toAdd_context.push_back(context);
		}
		return context;
	}

	//removes a context object from the input
	void Input::removeContext(Context* context) {
		if (context != nullptr && !withinVector(m_toRemove_context, context)) {
			m_toRemove_context.push_back(context);
		}
	}

	//finalizing adding contexts
	void Input::finalAddContext() {
		if (!m_toAdd_context.empty()) {
			for (auto& context : m_toAdd_context) {
				if (!withinVector(m_contexts, context)) {
					m_contexts.push_back(context);
				}
			}
			m_toAdd_context.clear();
		}
	}

	//finalizes removing contexts
	void Input::finalRemoveContext() {
		if (!m_toRemove_context.empty()) {
			for (auto& context : m_toRemove_context) {
				delete context;
				auto iter = std::find(m_contexts.begin(), m_contexts.end(), context);
				if (iter != m_contexts.end()) {
					m_contexts.erase(iter);
				}
			}
			m_toRemove_context.clear();
		}
	}

	//destructor
	Input::~Input() {
		finalAddContext();
		for (auto& context : m_contexts) {
			removeContext(context);
		}
		finalRemoveContext();
	}

}
