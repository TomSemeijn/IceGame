#ifndef REBOOTCOLLISIONSOLVE_H
#define REBOOTCOLLISIONSOLVE_H

#pragma once
#include <map>
#include <functional>
#include "Shape.h"
#include "Circle.h"
#include "Convex.h"
#include "Concave.h"
#include "CollisionInfo.h"
#include "Rect.h"

namespace reb {

	//contains an extensible static jump table for resolving collisions
	//also adds some default functions to this jump table and has a static function for AABB checks
	class CollisionSolve {
		//typedefs
		typedef std::pair<std::type_info, std::type_info> TypePair;
		typedef std::function <CollisionInfo(CollisionInfo&, const Shape*, const Shape*, Transform&, Transform&, Rect<float>&)> ShapeFunction;
	private:
		//the jumpTable
		static std::map<std::pair<size_t, size_t>, ShapeFunction> s_jumpTable;
	public:
		//registers a collision resolving function
		static void registerFunction(TypePair types, ShapeFunction function);
		//calls a collision resolution function
		static CollisionInfo resolveCollision(Rigidbody* a, Rigidbody* b, Rect<float>& intersection);

		//checks for AABB intersection
		static bool AABBIntersect(Rect<float> a, Rect<float> b, Rect<float>& intersection);
	private:
		//resolves circle to circle collision
		static CollisionInfo resolveCircleCircle(CollisionInfo&, const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
		//resolves circle to Convex collision
		static CollisionInfo resolveCircleConvex(CollisionInfo&, const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
		//resolves Convex to Convex collision
		static CollisionInfo resolveConvexConvex(CollisionInfo&, const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);

		//============ for Convex vs. Convex collision =================
		//finds index and penetration of the axis with the least penetration between both Convexs
		static int findLeastPenetrating(float& penetration, Convex* a, Convex* b, Transform& transA, Transform& transB);

		//from https://github.com/tutsplus/ImpulseEngine/blob/master/Collision.cpp 
		//(slightly edited to match this program and be more readable)
		//used to clip the line segment formed by the referenced edge1 and edge2 to only include points within
		//the upper and lower limits set by the upper and lower vertex of another face along the given normal
		static int clip(Vector2 normal, float c, Vector2& edge1, Vector2& edge2);
		//================================================================

		//resolves concave to circle collision
		static CollisionInfo resolveConcaveCircle(CollisionInfo&, const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
		//resolves concave to convex collision
		static CollisionInfo resolveConcaveConvex(CollisionInfo&, const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
		//resolves concave to concave collision
		static CollisionInfo resolveConcaveConcave(CollisionInfo&, const Shape*, const Shape*, Transform&, Transform&, Rect<float>&);
	};

}

#endif