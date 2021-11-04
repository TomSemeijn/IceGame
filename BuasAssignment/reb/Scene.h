#ifndef REBOOTSCENE_H
#define REBOOTSCENE_H

#pragma once
#include <set>
#include <unordered_set>
#include <vector>
#include "GameObj.h"
#include "Rigidbody.h"
#include "Animator.h"
#include "CollisionInfo.h"
#include "Tileset.h"
#include "Particle.h"
#include "Context.h"

namespace reb {

	class Game;

	//holds and updates all game objects
	class Scene {
		friend Game;

	private:
		//all game objects and standard components in the scene
		std::vector<GameObj*> m_gameObjects;
		std::vector<Animator*> m_animators;

		//a set of tilesets ordered by their layer
		std::set<Tileset> m_tilesets;

		//the input context managed by the scene
		Context* m_sceneContext = new Context();

		//keeps objects and standard components that should be removed at the end of this update
		std::vector<GameObj*> m_toRemove_gameObjects;
		std::vector<Animator*> m_toRemove_animators;
		//finalizes removal of objects and standard components
		void finalRemove();
		void finalRemoveObjects();
		void finalRemoveAnimators();

		//keeps objects and animators to be added at the end of this update
		std::vector<GameObj*> m_toAdd_gameObjects;
		std::vector<Animator*> m_toAdd_animators;
		//finalizes addition of objects and animators
		void finalAddition();
		void finalAdditionObjects();
		void finalAdditionAnimators();

		//updates rigidbodies
		void updateRigidbodies(float elapsed);

		//finds all the pairs of rigidbodies that collide
		void findRigidbodyPairs(std::vector<std::pair<GameObj*, Rect<float>>>& bodyObjects, std::vector<CollisionInfo>& collisions, std::vector<std::pair<Rigidbody*, Rigidbody*>>& triggers, float elapsed);
		//sets final collision and trigger info of the given two rigidbodies
		void setCollisionTriggerFinal(Rigidbody* a, Rigidbody* b, Rect<float>& intersection, Rect<float>& otherAABB, std::vector<CollisionInfo>& collisions, std::vector<std::pair<Rigidbody*, Rigidbody*>>& triggers, float elapsed);
	public:
		//updates the scene
		void update(float elapsed);

		//addition functions
		GameObj* addGameObj(GameObj* obj);
		Animator* addAnimator(Animator* anim);

		//removal functions
		void removeGameObj(GameObj* obj);
		void removeAnimator(Animator* anim);

		//tileset functions
		Tileset& addTileset(int tileWidth, int tileHeight, LayerIndex layer);
		void removeTileset(LayerIndex layer);
		Tileset& getTileset(LayerIndex layer);
		const Tileset& getConstTileset(LayerIndex layer)const;
		std::vector<LayerIndex> getTilesetLayers()const;
		bool hasTileset(LayerIndex layer)const;

		//const reference getters for rendering
		const std::vector<GameObj*>& getObjects()const;
		const std::vector<Animator*>& getAnimators()const;
		void getLightSources(std::vector<LightSource*>& vec, sf::FloatRect& viewBox)const;

		//returns the input context of the scene
		Context& getSceneContext();

		//returns true if the given object is within the scene
		bool hasObject(GameObj* obj);

		//clears the entire scene
		void clearScene();

		//destructor
		~Scene();
	};

}

#endif