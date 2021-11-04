#include "Scene.h"
#include "Functions.h"
#include "LightSource.h"
#include "CollisionSolve.h"
#include "IntersectionSolve.h"
#include "Container.h"
#include "ParticleEmitter.h"

namespace reb {

	//updates the scene
	void Scene::update(float elapsed) {

		//updates the animators
		for (auto& anim : m_animators) {
			anim->updateAnim();
		}

		//calls early update for the objects
		for (auto& obj : m_gameObjects) {
			obj->earlyUpdate(elapsed);
		}

		//updates the objects
		for (auto& obj : m_gameObjects) {
			((Container*)obj)->updateContainer(elapsed);
		}

		//updates the tiles
		for (auto& set : m_tilesets) {
			set.foreachTile([&elapsed](Tile*const& tile) {
				((Container*)tile)->updateContainer(elapsed);
			});
		}

		//updates the rigidbodies
		updateRigidbodies(elapsed);

		//calls late update for the objects
		for (auto& obj : m_gameObjects) {
			obj->lateUpdate(elapsed);
		}

		//adds everything that should be added
		finalAddition();

		//removes everything that should be removed
		finalRemove();

		//shrinks vectors to fit
		m_animators.shrink_to_fit();
		m_gameObjects.shrink_to_fit();
	}

	//returns the input context of the scene
	Context& Scene::getSceneContext() {
		return *m_sceneContext;
	}

	//adds all lightsources in the scene to the referenced vector
	void Scene::getLightSources(std::vector<LightSource*>& vec, sf::FloatRect& viewBox)const {
		for (auto& obj : m_gameObjects) {
			if (auto light = obj->getLight()) {
				if (light->onScreen(viewBox)) {
					vec.push_back(light);
				}
			}
		}
		for (auto& set : m_tilesets) {
			set.foreachTile([&vec, &viewBox](Tile*const& tile) {
				if (auto light = tile->getLight()) {
					if (light->onScreen(viewBox)) {
						vec.push_back(light);
					}
				}
			});
		}
	}

	//returns a const reference to the set of animators
	const std::vector<Animator*>& Scene::getAnimators()const {
		return m_animators;
	}

	//returns a const reference to the set of game objects
	const std::vector<GameObj*>& Scene::getObjects()const {
		return m_gameObjects;
	}

	//finds all the pairs of rigidbodies that collide or trigger
	void Scene::findRigidbodyPairs(std::vector<std::pair<GameObj*, Rect<float>>>& bodyObjects, std::vector<CollisionInfo>& collisions, std::vector<std::pair<Rigidbody*, Rigidbody*>>& triggers, float elapsed) {

		//verifies there is more than one game object in the scene
		for (auto objIter = bodyObjects.begin(); objIter < bodyObjects.end(); objIter++) {

			Rigidbody* body = objIter->first->getBody();

			//gets the aabb of the current body
			Rect<float> bodyAABB = objIter->second;

			//loops through all objects after the current one
			if (objIter < bodyObjects.end() - 1) {
				for (auto otherIter = objIter + 1; otherIter != bodyObjects.end(); otherIter++) {

					//adds collisions or triggers if any
					setCollisionTriggerFinal(body, otherIter->first->getBody(), bodyAABB, otherIter->second, collisions, triggers, elapsed);

				}
			}

			//loops through all tilesets
			for (auto& tileset : m_tilesets) {
				//skips if the current tileset isnt used in collision
				if (!tileset.m_hasCollision) { continue; }
				
				//finds the area of tiles the aabb of the body is in
				int minX = std::floor(bodyAABB.left / tileset.TILEWIDTH);
				int maxX = std::ceil(bodyAABB.getRight() / tileset.TILEWIDTH);
				int minY = std::floor(bodyAABB.top / tileset.TILEHEIGHT);
				int maxY = std::ceil(bodyAABB.getBottom() / tileset.TILEHEIGHT);

				LayerIndex currentLayer = tileset.LAYER;

				//loops through these tiles
				for (int x = minX; x <= maxX; x++) {
					for (int y = minY; y <= maxY; y++) {
						//checks if the current tile is not empty and has a rigidbody
						if (Tile* tile = tileset(x, y)) {
							if (auto tileBody = tile->m_rigidbody) {
								//adds collisions or triggers if any
								Rect<float> tileAABB = Rect<float>{ (float)x * (float)tileset.TILEWIDTH, (float)y * (float)tileset.TILEHEIGHT, (float)tileset.TILEWIDTH, (float)tileset.TILEHEIGHT };
								setCollisionTriggerFinal(body, tileBody, bodyAABB, tileAABB, collisions, triggers, elapsed);
							}
						}
					}
				}
			}

		}
	}

	//sets final collision and trigger info of the given two rigidbodies
	void Scene::setCollisionTriggerFinal(Rigidbody* a, Rigidbody* b, Rect<float>& bodyAABB, Rect<float>& otherAABB, std::vector<CollisionInfo>& collisions, std::vector<std::pair<Rigidbody*, Rigidbody*>>& triggers, float elapsed) {

		//skips if AABB's dont intersect
		Rect<float> intersection;
		if (!CollisionSolve::AABBIntersect(bodyAABB, otherAABB, intersection)) {
			return;
		}

		//skips if neither object is movable
		if (a->getInverseMass() < EPSILON && b->getInverseMass() < EPSILON) {
			return;
		}

		//skips if the owning objects can't collide with eachother
		if (!a->getOwner()->canCollide(b->getOwner()) || !b->getOwner()->canCollide(a->getOwner())) {
			return;
		}

		//if both rigidbodies are solid, physics are applied
		if (a->SOLID && b->SOLID) {
			//gets collision info
			CollisionInfo info = CollisionSolve::resolveCollision(a, b, intersection);
			//adds collision info if there are any found contact points
			if (info.contactPoints.size() > 0) {
				info.setMaterialInfo(elapsed);
				collisions.push_back(info);
			}
		}
		//if either or both of the rigidbodies are not solid, trigger information is updated
		else {
			//checks if the two rigidbodies intersect
			if (IntersectionSolve::intersect(a, b, intersection)) {
				//adds the pair of bodies to the triggers vector
				triggers.emplace_back(std::make_pair(a, b));
			}
		}
	}

	//adds a tileset to the scene
	Tileset& Scene::addTileset(int tileWidth, int tileHeight, LayerIndex layer) {
		m_tilesets.emplace(tileWidth, tileHeight, layer);
		getTileset(layer).setOwner(this);
		return getTileset(layer);
	}

	//removes a tileset from the scene
	void Scene::removeTileset(LayerIndex layer) {
		m_tilesets.erase(std::find_if(m_tilesets.begin(), m_tilesets.end(), [&layer](const Tileset& tileset) {
			return tileset.LAYER == layer;
		}));
	}

	//returns the tileset with the given layer
	Tileset& Scene::getTileset(LayerIndex layer) {
		//even though the tilesets are in a set, a reference to them can still be returned because the tilesets are
		//sorted by their constant layer index. Though there would be trouble if that were to change

		//hacky way of removing const
		return const_cast<Tileset&>(*std::find_if(m_tilesets.begin(), m_tilesets.end(), [&layer](const Tileset& set) {
			return set.LAYER == layer;
		}));
	}

	//returns a const reference to the requested tileset
	const Tileset& Scene::getConstTileset(LayerIndex layer)const {
		return *std::find_if(m_tilesets.begin(), m_tilesets.end(), [&layer](const Tileset& set) {
			return set.LAYER == layer;
		});
	}

	//returns a vector of every layer the scene has a tileset on
	std::vector<LayerIndex> Scene::getTilesetLayers()const {
		std::vector<LayerIndex> layers{};
		for (auto& tileset : m_tilesets) {
			layers.push_back(tileset.LAYER);
		}
		return layers;
	}

	//returns true if the scene has a tileset with the given layer
	bool Scene::hasTileset(LayerIndex layer)const {
		return std::find_if(m_tilesets.begin(), m_tilesets.end(), [&layer](const Tileset& set) {return set.LAYER == layer; }) != m_tilesets.end();
	}

	//updates rigidbodies
	void Scene::updateRigidbodies(float elapsed) {

		//gets all the rigidbody objects
		std::vector<std::pair<GameObj*, Rect<float>>> bodyObjects;
		for (auto& obj : m_gameObjects) {
			if (auto body = obj->getBody()) {
				bodyObjects.push_back(std::make_pair(obj, body->getShape()->getAABB(*obj->getTransform())));
			}
		}

		//applies half of the current forces to every body
		for (auto& obj : bodyObjects) {
			obj.first->getBody()->applyForces(elapsed * 0.5f);
		}
		
		//finds all pairs of rigidbodies that collide
		std::vector<CollisionInfo> pairs{};
		pairs.reserve(bodyObjects.size() * (bodyObjects.size() - 1));
		std::vector<std::pair<Rigidbody*, Rigidbody*>> triggers;
		findRigidbodyPairs(bodyObjects, pairs, triggers, elapsed);

		//applies impulses for all found pairs
		for (auto& info : pairs) {
			info.addImpulse(elapsed);
		}

		//applies the resulting velocities and half of their forces
		for (auto& obj : bodyObjects) {
			auto body = obj.first->getBody();
			body->applyVelocities(elapsed);
			body->applyForces(elapsed * 0.5f);
		}

		//corrects positions for sinking
		for (auto& pair : pairs) {
			pair.positionalCorrection();
		}

		//sets all triggers as inside of eachother
		for (auto& trigger : triggers) {
			//sets in both rigidbodies the other as isInside
			trigger.first->setInside(trigger.second);
			trigger.second->setInside(trigger.first);
		}

		//updates trigger information for all the rigidbodies
		for (auto& obj : bodyObjects) {
			obj.first->getBody()->updateTriggers();
		}

		//calls onTouch functions on all collision pairs
		for (auto& pair : pairs) {
			auto objA = pair.a->getOwner();
			auto objB = pair.b->getOwner();
			for (auto& point : pair.contactPoints) {
				objA->onTouch(objB, point, pair.normal);
				objB->onTouch(objA, point, pair.normal);
			}
		}
	}

	//adds a game object to the scene
	GameObj* Scene::addGameObj(GameObj* obj) {
		//verifies the object isnt a nullptr
		if (obj != nullptr && !withinVector(m_gameObjects, obj) && !withinVector(m_toAdd_gameObjects, obj)) {
			//adds the object to the vector
			m_toAdd_gameObjects.push_back(obj);
			//sets this scene as the scene within the object
			obj->setOwner(this);
		}
		return obj;
	}

	//adds an animator to the scene
	Animator* Scene::addAnimator(Animator* anim) {
		//verifies the animator isnt nullptr
		if (anim != nullptr && !withinVector(m_animators, anim) && !withinVector(m_toAdd_animators, anim)) {
			//adds the animator to the scene
			m_toAdd_animators.push_back(anim);
			//sets the scene of the animator to this
			anim->setOwner(this);
		}
		//returns the animator
		return anim;
	}

	//finalizes addition of objects and animators
	void Scene::finalAddition() {
		finalAdditionObjects();
		finalAdditionAnimators();
	}

	//finalizes addition of objects
	void Scene::finalAdditionObjects() {
		if (m_toAdd_gameObjects.empty()) { return; }
		//loops through all objects to add
		for (auto& obj : m_toAdd_gameObjects) {
			m_gameObjects.push_back(obj);
		}
		//clears the vector
		m_toAdd_gameObjects.clear();
	}

	//finalizes addition of animators
	void Scene::finalAdditionAnimators() {
		if (m_toAdd_animators.empty()) { return; }
		//loops through all animators to add
		for (auto& anim : m_toAdd_animators) {
			//adds the animator to the scene
			m_animators.push_back(anim);
		}
		//clears the vector
		m_toAdd_animators.clear();
	}

	//removes a game object from the scene
	void Scene::removeGameObj(GameObj* obj) {
		if (!withinVector(m_toRemove_gameObjects, obj)) {
			m_toRemove_gameObjects.push_back(obj);
		}
	}

	//removes an animator from the scene
	void Scene::removeAnimator(Animator* anim) {
		if (!withinVector(m_toRemove_animators, anim)) {
			m_toRemove_animators.push_back(anim);
		}
	}

	//returns true if the given object is within the scene
	bool Scene::hasObject(GameObj* obj) {
		return std::find(m_gameObjects.begin(), m_gameObjects.end(), obj) != m_gameObjects.end();
	}

	//finalizes removal of objects and standard components
	void Scene::finalRemove() {
		finalRemoveObjects();
		finalRemoveAnimators();
	}

	//finalizes removal of objects
	void Scene::finalRemoveObjects() {
		if (m_toRemove_gameObjects.empty()) { return; }
		for (auto& remove : m_toRemove_gameObjects) {

			//gets an iterator to the pointer in the set
			auto objIter = std::find(m_gameObjects.begin(), m_gameObjects.end(), remove);
			//verifies the pointer was found
			if (objIter != m_gameObjects.end()) {
				//removes the object from the set
				swapPopEl(m_gameObjects, remove);
			}

			//removes the object from any animators that may have it
			for (auto& anim : m_animators) {
				anim->removeObject(remove);
			}

			//frees the memory
			delete remove;
		}
		m_toRemove_gameObjects.clear();
	}

	//finalizes removal of animators
	void Scene::finalRemoveAnimators() {
		if (m_toRemove_animators.empty()) { return; }
		for (auto& remove : m_toRemove_animators) {
			//removes the animator from the set
			swapPopEl(m_animators, remove);

			//frees memory
			delete remove;
		}
		m_toRemove_animators.clear();
	}

	//clears the entire scene
	void Scene::clearScene() {

		//adds anything that might still be added so that can be removed as well
		finalAddition();
		//adds every component and object to be removed
		for (auto& obj : m_gameObjects) {
			removeGameObj(obj);
		}
		for (auto& anim : m_animators) {
			removeAnimator(anim);
		}
		//frees all the memory
		finalRemove();

		//frees context memory
		delete m_sceneContext;

		//resets the input context
		m_sceneContext = new Context();

		//clears all the vectors
		m_gameObjects.clear();
		m_animators.clear();
		m_tilesets.clear();
	}

	//destructor
	Scene::~Scene() {
		
		//basically clearScene is what the destructor used to be but it also clears the vectors
		//so rather than have it be duplicated the destructor just calls clearScene now
		clearScene();
		
		//frees the context
		delete m_sceneContext;

	}

}