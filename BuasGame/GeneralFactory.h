#pragma once
#include <reb/Tile.h>
#include <reb/ContentLoader.h>
#include "level.h"


using namespace reb;

//returns the torch tile prototype
Tile* getTorchPrototype(Content* content, Level* level = nullptr);

//adds a firefly emitter to the scene
GameObj* addFireflyEmitter(Scene& scene, Content* content, int flyAmount, std::pair<float, float> lifespanRange, std::pair<Vector2, Vector2> spawnRange);