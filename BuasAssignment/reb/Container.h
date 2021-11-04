#ifndef REBOOTCONTAINER_H
#define REBOOTCONTAINER_H

#pragma once
#include <unordered_set>
#include "Component.h"
#include "Vector2.h"
#include "OwnedObject.h"

namespace reb {

	extern enum LayerIndex : int;

	class Scene;
	class Transform;
	class Rigidbody;
	class Drawable;
	class LightSource;
	class ParticleEmitter;

	//defines a container of components that are combined to form entities within the game
	class Container {

		//sets the scene as a friend so it can access the update function
		friend Scene;
		//sets rigidbody as a friend so it can call the triggerEnter and triggerLeave functions
		friend Rigidbody;

	private:
		//the transform of the container
		Transform* m_transform;

		//the components managed and used by the main systems of the game
		Rigidbody* m_rigidbody = nullptr;
		Drawable* m_sprite = nullptr;
		LightSource* m_lightsource = nullptr;
		ParticleEmitter* m_particleEmitter = nullptr;

		//true if the component should be cleared
		bool m_clearBody = false;
		bool m_clearSprite = false;
		bool m_clearLight = false;
		bool m_clearEmitter = false;

		//not nullptr if the current component should be replaced by it
		Rigidbody* m_replacement_rigidbody = nullptr;
		Drawable* m_replacement_sprite = nullptr;
		LightSource* m_replacement_lightsource = nullptr;
		ParticleEmitter* m_replacement_particleEmitter = nullptr;

		//finalizes removing and setting main components
		void finalizeMainComponents();

		//custom components only used for updating
		std::vector<Component*> m_components{};

		//the layer of the container
		LayerIndex m_layer = s_defaultLayer;

		//updates the container
		void updateContainer(float elapsed);

		//keeps a set of components to remove at the end of the update
		std::vector<Component*> m_toRemove_components;
		void finalRemoveComponents();

		//components that should be added at the end of the update
		std::vector<Component*> m_toAdd_components;
		void finalAddComponents();

	protected:

		//allows child classes to transform themselves even if they shouldn't be able to
		void forcePos(Vector2 newPos);
		void forceSize(float newSize);
		void forceRotation(float newRotation);

		//allows child classes to add to the update function
		virtual void update(float elapsed) {};

		//trigger callbacks (behavior can be added by child classes)
		virtual void triggerEnter(Container* other) {};
		virtual void triggerLeave(Container* other) {};

		//gets called when the rigidbody (if solid) touches another solid rigidbody (behavior can be added by child classes)
		virtual void onTouch(Container* other, Vector2 contactPoint, Vector2 contactNormal) {};

		//returns true if this object can collide with the other (can be overriden by child classes to apply collision layering)
		virtual bool canCollide(Container* other) { return true; }

		//copy constructor
		Container(const Container& original);

	public:
		//enum to determine which component defines the bounds of the object
		enum BoundType {
			BOUNDS_SPRITE,
			BOUNDS_RIGIDBODY
		};
		const BoundType BOUNDTYPE;

		//the layer used as a default
		static LayerIndex s_defaultLayer;

		//constructors
		Container(BoundType boundType = BoundType::BOUNDS_RIGIDBODY);
		Container(Transform* transform, BoundType boundType = BoundType::BOUNDS_RIGIDBODY);

		//getters
		Transform* getTransform()const;
		Drawable* getSprite()const;
		LightSource* getLight()const;
		Rigidbody* getBody()const;
		ParticleEmitter* getEmitter()const;
		LayerIndex getLayer()const;
		std::vector<Component*>& getComponents();
		const std::vector<Component*>& getConstComponents()const;

		//setters
		void setSprite(Drawable* newSprite);
		void setLight(LightSource* newLight);
		void setBody(Rigidbody* newBody);
		void setEmitter(ParticleEmitter* newEmitter);
		void addComponent(Component* newComp);
		void setLayer(LayerIndex newLayer);

		//clear-ers
		void clearSprite();
		void clearLight();
		void clearBody();
		void clearEmitter();
		void clearComponents();
		void removeComponent(Component* toRemove);

		//copies main components from the referenced container
		void copyMainComponentsFrom(const Container& other);

		//removes itself
		virtual void destroy() = 0;

		//returns the scene
		virtual Scene* getScene() = 0;

		//destructor
		virtual ~Container();
	};

}

#endif