#ifndef REBOOTVIEW_H
#define REBOOTVIEW_H

#pragma once
#include "Container.h"
#include "OwnedObject.h"
#include "ViewComp.h"
#include "Rect.h"
#include "Shape.h"
#include "Shader.h"

namespace reb {

	class GUI;

	//a view can be any element within a gui, it can be given components to add behavior and/or visuals
	class View : public OwnedObject<GUI>, public OwnedObject<View> {
		//sets GUI as a friend so it can call the private update and draw functions
		friend GUI;
	private:
		//true if the view is active, in which case it's drawn and updated
		bool m_active = true;

		//the position of the mouse relative to the view
		mutable Vector2 m_localMousePos;

		//booleans for figuring out when to call the event functions
		mutable bool m_wasHover = false;
		mutable bool m_isHover = false;
		mutable bool m_wasClicked = false;
		mutable bool m_isClicked = false;
		mutable bool m_wasActive = false;
		mutable bool m_mouseWasClicked = false;
		mutable bool m_mouseIsClicked = false;

		//vector of child views
		std::vector<View*> m_children;
		//vector of components
		std::vector<ViewComp*> m_components;

		//the transform of the view
		Transform m_transform;
		//main aabb of the view
		Rect<float> m_aabb;
		//optional more precise shape of the view
		Shape* m_target;

		//manages removing components safely
		std::vector<ViewComp*> m_toRemove_components;
		void finalRemoveComponents();

		//manages adding components safely
		std::vector<ViewComp*> m_toAdd_components;
		void finalAddComponents();

		//manages removing children safely
		std::vector<View*> m_toRemove_children;
		void finalRemoveChildren();

		//manages adding children safely
		std::vector<View*> m_toAddChildren;
		void finalAddChildren();

		//private basic draw and update functions
		void updateView(float elapsed);
		void drawView(sf::RenderTarget& window, Shader& shader, float scale)const;
	protected:
		//event functions for child classes
		virtual void onClick() { };
		virtual void onHeld() { };
		virtual void onRelease() { };
		virtual void onActivate() {};
		virtual void onDeactivate() {};
		virtual void onMouseEnter() { };
		virtual void onMouseLeave() { };
		virtual void update(float elapsed) {};

		//draws the view
		virtual void draw(sf::RenderTarget& window, Shader& shader, float scale)const {};

		//returns the mouse position relative to this view
		Vector2 getLocalMousePos()const;
	public:
		//true if this view should pause the scene when active
		const bool PAUSES;

		//true if this view should show the cursor when active
		const bool SHOWCURSOR;

		//constructor
		View(Vector2 size, Shape* target = nullptr, Vector2 pos = Vector2(), bool pauses = false, bool showCursor = false);

		//component functions
		void addComp(ViewComp* comp);
		void removeComp(ViewComp* comp);

		//child functions
		View* addChild(View* child);
		void removeChild(View* child);

		//activation functions
		void activate();
		void deactivate();
		void toggleActive();

		//position functions
		Vector2 getPos()const;
		void setPos(Vector2 newPos);
		void setPos(float newX, float newY);

		//scale functions
		float getScale()const;
		void setScale(float newScale);

		//returns the aabb of the view
		Rect<float> getAABB()const;

		//destroys the view
		void destroy()override;

		//destructor
		virtual ~View();
	};

}

#endif