#include "View.h"
#include "GUI.h"

namespace reb {

	//constructor
	View::View(Vector2 size, Shape* target, Vector2 pos, bool pauses, bool showCursor) : m_aabb{ 0, 0, size.x, size.y }, m_transform{ true, true, false, pos }, m_target{ target }, PAUSES{ pauses }, SHOWCURSOR{ showCursor }{};

	//activates the view
	void View::activate() {
		if (!m_active) {
			m_active = true;
			onActivate();
		}
	}

	//deactivates the view
	void View::deactivate() {
		if (m_active) {
			m_active = false;
			onDeactivate();
		}
	}

	//toggles the activation
	void View::toggleActive() {
		m_active = !m_active;
		if (m_active) {
			onActivate();
		}
		else {
			onDeactivate();
		}
	}

	//safely removes components at the end of an update
	void View::finalRemoveComponents() {
		if (!m_toRemove_components.empty()) {
			for (auto& remove : m_toRemove_components) {
				delete remove;
				if (withinVector(m_components, remove)) {
					swapPopEl(m_components, remove);
				}
			}
			m_toRemove_components.clear();
		}
	}

	//safely adds components at the end of an update
	void View::finalAddComponents() {
		if (!m_toAdd_components.empty()) {
			for (auto& add : m_toAdd_components) {
				if (!withinVector(m_components, add)) {
					m_components.push_back(add);
				}
			}
		}
	}

	//safely removes children at the end of an update
	void View::finalRemoveChildren() {
		if (!m_toRemove_children.empty()) {
			for (auto& remove : m_toRemove_children) {
				delete remove;
				if (withinVector(m_children, remove)) {
					swapPopEl(m_children, remove);
				}
			}
			m_toRemove_children.clear();
		}
	}

	//safely adds children at the end of an update
	void View::finalAddChildren() {
		if (!m_toAddChildren.empty()) {
			for (auto& add : m_toAddChildren) {
				if (!withinVector(m_children, add)) {
					m_children.push_back(add);
				}
			}
			m_toAddChildren.clear();
		}
	}

	//returns the mouse position relative to this view
	Vector2 View::getLocalMousePos()const {
		return m_localMousePos;
	}

	//draws the view
	void View::drawView(sf::RenderTarget& window, Shader& shader, float scale)const {
		float realScale = scale * getScale();
		//calls virtual draw function
		draw(window, shader, realScale);
		//draws components
		for (auto& comp : m_components) {
			comp->draw(window, shader, realScale);
		}
		//draws children
		for (auto& view : m_children) {
			view->drawView(window, shader, realScale);
		}
		//sets local mouse position
		if (sf::RenderWindow* cast = dynamic_cast<sf::RenderWindow*>(&window)) {
			Transform* realTransf = m_transform.clone();
			realTransf->setPos(getPos());
			m_localMousePos = realTransf->getLocalSpace(cast->mapPixelToCoords(sf::Mouse::getPosition(*cast)));
			delete realTransf;
		}
	}

	//updates the view and its children and components
	void View::updateView(float elapsed) {
		//sets mouse event booleans
		m_isHover = m_aabb.withinRect(m_localMousePos) && (m_target == nullptr || m_target->withinShape(m_localMousePos));
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			m_mouseIsClicked = true;
			if (m_wasClicked || (!m_mouseWasClicked && m_isHover)) {
				m_isClicked = true;
			}
		}
		//calls events if necessary
		if (m_isClicked) {
			if (!m_wasClicked) {
				onClick();
			}
			else {
				onHeld();
			}
		}
		else if (m_wasClicked) {
			onRelease();
		}
		if (m_isHover && !m_wasHover) {
			onMouseEnter();
		}
		else if (!m_isHover && m_wasHover) {
			onMouseLeave();
		}
		//updates components
		for (auto& comp : m_components) {
			comp->update(elapsed);
		}
		//updates children
		for (auto& view : m_children) {
			view->updateView(elapsed);
		}
		//calls virtual update function
		update(elapsed);
		//finalizes adding and removing components
		finalAddComponents();
		finalRemoveComponents();
		//finalizes adding and removing children
		finalAddChildren();
		finalRemoveChildren();
		//updates event booleans
		m_wasActive = m_active;
		m_wasClicked = m_isClicked;
		m_wasHover = m_isHover;
		m_mouseWasClicked = m_mouseIsClicked;
		m_isClicked = false;
		m_isHover = false;
		m_mouseIsClicked = false;
	}

	//adds a components
	void View::addComp(ViewComp* comp) {
		if (!withinVector(m_toAdd_components, comp)) {
			m_toAdd_components.push_back(comp);
			comp->setOwner(this);
		}
	}

	//removes a component
	void View::removeComp(ViewComp* comp) {
		if (!withinVector(m_toRemove_components, comp)) {
			m_toRemove_components.push_back(comp);
		}
	}

	//adds a child
	View* View::addChild(View* child) {
		//makes sure the view has no gui owner
		if (!((OwnedObject<GUI>*)child)->hasOwner()) {
			//adds the view
			if (!withinVector(m_toAddChildren, child)) {
				m_toAddChildren.push_back(child);
				((OwnedObject<View>*)child)->setOwner(this);
			}
		}
		//throws an exception otherwise
		else {
			throw std::out_of_range("Tried to add a view that belongs to a gui to another view as a child");
		}
		return child;
	}

	//removes a child
	void View::removeChild(View* child) {
		if (!withinVector(m_toRemove_children, child)) {
			m_toRemove_children.push_back(child);
		}
	}

	//returns the position of the view
	Vector2 View::getPos()const {
		Vector2 myPos = m_transform.getPos();
		if (OwnedObject<View>::hasOwner()) {
			myPos += OwnedObject<View>::getOwner()->getPos();
		}
		return myPos;
	}

	void View::setPos(Vector2 newPos) {
		m_transform.setPos(newPos);
	}

	void View::setPos(float newX, float newY) {
		setPos(Vector2(newX, newY));
	}

	//scale functions
	float View::getScale()const {
		return m_transform.getScale();
	}

	void View::setScale(float newScale) {
		m_transform.setScale(newScale);
	}

	//returns the aabb of the view
	Rect<float> View::getAABB()const {
		return m_aabb + getPos();
	}

	//destroys the view
	void View::destroy() {
		if (OwnedObject<GUI>::hasOwner()) {
			OwnedObject<GUI>::getOwner()->removeView(this);
		}
		else if (OwnedObject<View>::hasOwner()) {
			OwnedObject<View>::getOwner()->removeChild(this);
		}
	}

	//destructor
	View::~View() {
		//frees memory of children
		for (auto& child : m_children) {
			delete child;
		}
		//frees memory of components
		finalAddComponents();
		for (auto& comp : m_components) {
			removeComp(comp);
		}
		finalRemoveComponents();
		//frees memory of target shape if there is one
		if (m_target != nullptr) {
			delete m_target;
		}
	}

}