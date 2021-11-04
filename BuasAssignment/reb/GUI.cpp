#include "GUI.h"

namespace reb {

	//reutrns the input context of the gui
	Context& GUI::getGuiContext() {
		return *m_guiContext;
	}

	//finalizes the removal of views at the end of an update
	void GUI::finalRemoveViews() {
		if (!m_toRemove_views.empty()) {
			for (auto& remove : m_toRemove_views) {
				delete remove;
				if (withinVector(m_views, remove)) {
					swapPopEl(m_views, remove);
				}
			}
			m_toRemove_views.clear();
		}
	}

	//finalizes the addition of views at the end of an update
	void GUI::finalAddViews() {
		if (!m_toAdd_views.empty()) {
			for (auto& add : m_toAdd_views) {
				if (!withinVector(m_views, add)) {
					m_views.push_back(add);
				}
			}
			m_toAdd_views.clear();
		}
	}

	//constructor
	GUI::GUI() : m_size{ config.getConfigValue<float>(Config::GUIWIDTH), config.getConfigValue<float>(Config::GUIHEIGHT) } {}

	//draws all the views
	void GUI::draw(sf::RenderTarget& window, Shader& shader)const {
		if (auto cast = dynamic_cast<sf::Window*>(&window)) {
			//shows or hides the cursor
			cast->setMouseCursorVisible(m_showCursor);
			//traps the cursor within the window when hidden
			cast->setMouseCursorGrabbed(!m_showCursor);
		}

		//gets the current view of the window
		sf::View oldView = window.getView();
		//sets the view of the window to match the gui
		sf::View guiView(sf::FloatRect(0, 0, m_size.x, m_size.y));
		guiView.setViewport(oldView.getViewport());
		window.setView(guiView);
		//draws all the views in the gui
		for (auto& view : m_views) {
			if (view->m_active) {
				view->drawView(window, shader, 1);
			}
		}
		//sets the view of the window back to what it was
		window.setView(oldView);
	}

	//updates all the views
	void GUI::update(float elapsed) {
		m_scenePaused = false;
		m_showCursor = false;
		for (auto& view : m_views) {
			if (view->m_active) {
				if (view->PAUSES) {
					m_scenePaused = true;
				}
				if (view->SHOWCURSOR) {
					m_showCursor = true;
				}
				view->updateView(elapsed);
			}
		}
		finalAddViews();
		finalRemoveViews();
	}

	//adds a view
	View* GUI::addView(View* view) {
		//makes sure the view has no view owner
		if (!((OwnedObject<View>*)view)->hasOwner()) {
			//adds the view
			if (!withinVector(m_toAdd_views, view)) {
				m_toAdd_views.push_back(view);
				((OwnedObject<GUI>*)view)->setOwner(this);
			}
		}
		//throws an exception otherwise
		else {
			throw std::out_of_range("Tried to add a view that belongs to another view as a child to a GUI");
		}
		return view;
	}

	//removes a view
	void GUI::removeView(View* view) {
		if (!withinVector(m_toRemove_views, view)) {
			m_toRemove_views.push_back(view);
		}
	}

	//returns the size of the gui
	Vector2 GUI::getSize() {
		return m_size;
	}

	//clears everything from the gui
	void GUI::clearGUI() {
		//frees memory of all the views
		finalAddViews();
		for (auto& view : m_views) {
			removeView(view);
		}
		finalRemoveViews();

		//frees context
		delete m_guiContext;

		//resets the guiContext
		m_guiContext = new Context();

		//clears vectors
		m_views.clear();
	}

	//destructor
	GUI::~GUI() {
		
		//basically clearGUI is what the destructor used to be but it also clears the vectors
		//so rather than have it be duplicated the destructor just calls clearGUI now
		clearGUI();

		//frees the context
		delete m_guiContext;

	}

}